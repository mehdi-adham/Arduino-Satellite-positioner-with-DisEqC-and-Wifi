#include "Diseqc.h"
extern "C"
{
#include <gpio.h>
#include <user_interface.h>
}
#include <Arduino.h>

// Globals

#define USE_IRAM_ATTR ICACHE_RAM_ATTR

#define ONCE 0

static ETSTimer timer;

volatile DiseqcParams_t diseqcparams;

// Interrupts -------------------

/// Interrupt handler for when the timer runs out.
/// It signals to the library that capturing of Diseqc data has stopped.
static void USE_IRAM_ATTR read_timeout(void *arg __attribute__((unused)))
{
  os_intr_lock();
  if (diseqcparams.rawlen)
    diseqcparams.rcvstate = kStopState;
  os_intr_unlock();
}

/// Interrupt handler for changes on the GPIO pin handling incoming Diseqc messages.
static void USE_IRAM_ATTR decode_intr()
{
  uint32_t now = micros();
  static uint32_t start = 0;

  uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
  os_timer_disarm(&timer);
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

  //برای کاهش دستورالعملهای استفاده شده در
  //IRAM ،
  // یک نسخه محلی از
  //rawlen
  //بگیرید
  uint16_t rawlen = diseqcparams.rawlen;
  uint16_t pulseWidth = 0;

  if (rawlen >= diseqcparams.bufsize)
  {
    diseqcparams.overflow = true;
    diseqcparams.rcvstate = kStopState;
  }

  if (diseqcparams.rcvstate == kStopState)
    return;

  if (diseqcparams.rcvstate == kIdleState)
  {
    diseqcparams.rcvstate = kMarkState;
    diseqcparams.rawbuf[rawlen] = 1;
    /**/
    diseqcparams.rawlen++;
  }
  else
  {
    if (now < start)
      pulseWidth = (UINT32_MAX - start + now) / kRawTick;
    else
      pulseWidth = (now - start) / kRawTick;
    /**/
    if (pulseWidth > 100)
    {
      diseqcparams.rawbuf[rawlen] = pulseWidth;
      diseqcparams.rawlen++;
    }
  }
  //diseqcparams.rawlen++;

  start = now;

  os_timer_arm(&timer, diseqcparams.timeout, ONCE);
}

// Function's -------------------

// Start of Diseqc class -------------------

/// Class constructor
/// Args:
/// @param[in] recvpin The GPIO pin the Diseqc receiver module's data pin is
///   connected to.
/// @param[in] bufsize Nr. of entries to have in the capture buffer.
///   (Default: kRawBuf)
/// @param[in] timeout Nr. of milli-Seconds of no signal before we stop
///   capturing data. (Default: kTimeoutMs)
Diseqc::Diseqc(const uint16_t recvpin,
               const uint16_t bufsize, const uint8_t timeout)
{
  diseqcparams.recvpin = recvpin;
  diseqcparams.bufsize = bufsize;
  diseqcparams.timeout = std::min(timeout, (uint8_t)kMaxTimeoutMs);
  diseqcparams.rawbuf = new uint16_t[bufsize];

  if (diseqcparams.rawbuf == NULL)
  {
#ifndef UNIT_TEST
    ESP.restart(); // Mem alloc failure. Reboot.
#endif
  }
}

/// Class destructor
/// Cleans up after the object is no longer needed.
/// e.g. Frees up all memory used by the various buffers, and disables any
/// timers or interrupts used.
Diseqc::~Diseqc(void)
{
  disableDiseqcIn();
  delete[] diseqcparams.rawbuf;
}

bool Diseqc::decode(decode_results *results)
{
  // Proceed only if an Diseqc message been received.
  if (diseqcparams.rcvstate != kStopState)
    return false;

  diseqcparams.rawbuf[diseqcparams.rawlen] = 0;

  // bool resumed = false; // Flag indicating if we have resumed.

  results->rawbuf = diseqcparams.rawbuf;
  results->rawlen = diseqcparams.rawlen;
  results->overflow = diseqcparams.overflow;

  // Reset any previously partially processed results.
  results->bits = 0;
  results->framming = 0;
  results->address = 0;
  results->command = 0;
  results->data = 0;

  // convert rawbuf to byte
  uint8_t packet[9];
  int bit = 7;
  uint8_t byte = 0;

  packet[0] = 0;
  packet[1] = 0;
  packet[2] = 0;
  packet[3] = 0;
  packet[4] = 0;
  packet[5] = 0;

  for (uint16_t offset = kStartOffset; offset <= results->rawlen; offset++)
  {
    // GET parity Bit if needed.
    if (bit < 0)
    {
      /*
      is valid Parity bit =>return true/false or parity = false
      if(!ValidPairty())
       return false; //Byte is incorrect
      or
      parity = false;
      */
      byte++;
      bit = 7;
    }
    // GET Data Bit
    else
    {
      if (results->rawbuf[offset] > 600) //IS bit 1
        packet[byte] |= (1 << bit);
      else //IS bit 0
        packet[byte] &= ~(1 << bit);
      bit--;
    }
  }

  results->bits = results->rawlen;
  results->framming = packet[0];
  results->address = packet[1];
  results->command = packet[2];
  results->data = (0x0000ff & packet[3]) +
                  (0x00ff00 & (packet[4]) << 8) +
                  (0xff0000 & (packet[5]) << 16);

  return true;
  /*******************/
}

/// Set up and (re)start the Diseqc capture mechanism.
/// @param[in] pullup A flag indicating should the GPIO use the internal pullup
/// resistor. (Default: `false`. i.e. No.)
void Diseqc::begin(const bool pullup)
{
  if (pullup)
    pinMode(diseqcparams.recvpin, INPUT_PULLUP);
  else
    pinMode(diseqcparams.recvpin, INPUT);
  // Initialize state machine variables
  resume();

  // Initialize ESP8266 timer.
  os_timer_disarm(&timer);
  os_timer_setfn(&timer, reinterpret_cast<os_timer_func_t *>(read_timeout), NULL);

  // Attach Interrupt
  attachInterrupt(diseqcparams.recvpin, decode_intr, CHANGE);
}

/// Stop collection of any received Diseqc data.
/// Disable any timers and interrupts.
void Diseqc::disableDiseqcIn(void)
{
  os_timer_disarm(&timer);
  detachInterrupt(diseqcparams.recvpin);
}

/// Resume collection of received Diseqc data.
/// @note This is required if `decode()` is successful
///   not set when the class was instanciated.
/// @see Diseqc class constructor
void Diseqc::resume(void)
{
  diseqcparams.rcvstate = kIdleState;
  diseqcparams.rawlen = 0;
  diseqcparams.overflow = false;
}
