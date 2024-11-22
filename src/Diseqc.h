#ifndef DISEQCRECV_H_
#define DISEQCRECV_H_

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <stddef.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "Diseqc.h"

// Constants

// Diseqc Constants

//Framing byte Function
#define Master_First_transmission_No_reply 0xE0
#define Master_Repeated_First_transmission_No_reply 0xE1
#define Master_First_transmission_Reply_required 0xE2
#define Master_Repeated_First_transmission_Reply_required 0xE3
/*
#define Slave_Ok                        0xE4
#define Slave_not_supported             0xE5
#define Slave_Parity_Error_detected     0xE6
#define Slave_Command_not_recognised    0xE7
*/

//Address byte Function
/*
#define Any_Device        0x00 //Master to all devices
#define Any_LNB           0x10 //Switcher or SMATV (Master to all...)
#define LNB               0x11
#define LNB_with_Loop     0x12 //LNB with Loop-through switching
#define Switcher            0x14 //Switcher (d.c. blocking)
#define Switcher_with_Loop  0x15 //Switcher with d.c. Loop-through
#define SMATV               0x18
#define Any_Polariser       0x20 //Master to all Polarisers
#define Linear_Polarisation       0x21 //Linear Polarisation (Skew) Controller
*/
#define Any_Positioner 0x30 //Master to all Positioners
#define Polar_Azimuth_Positioner 0x31
#define Elevation_Positioner 0x32
/*
#define Any_Installer_Aid          0x40 //Master to all Installer Aids
#define analogue_value             0x41
//#define re_allocations             0x60 - 0x69 //Family reserved for address re-allocations
#define Any_Intelligent             0x70 //Any Intelligent Slave Interfaces (Master to all)
//#define Interface             0x71 //Interface for subscriber controlled headends
//#define OEM_Extensions             0xF0 - 0xF9 //Reserved for OEM Extensions
*/

//Command byte Function
#define Reset 0x00       //Reset DiSEqCä microcontroller
#define Clr_Reset 0x01   //Clear the “Reset” flag
#define Standby 0x02     //Switch peripheral power supply off
#define Power_on 0x03    //Switch peripheral power supply on
#define Set_Contend 0x04 //Set Contention flag
#define Contend 0x05     //Return address only if Contention flag is set
#define Clr_Contend 0x06 //Clear Contention flag
#define Address 0x07     //Return address unless Contention flag is set
#define Move_C 0x08      //Change address only if Contention flag is set
#define Move 0x09        //Change address unless Contention flag is set
//0F Reserved
#define Status 0x10 //Read Status register flags
#define Config 0x11 //Read Configuration flags (peripheral hardware)
/*
#define Switc_0          0x14 //Read Switching state flags (Committed port) 
#define Switch_1          0x15 //Read Switching state flags (Uncommitted port)
#define Switch_2         0x16 //expansion option
#define Switch_3         0x17 //expansion option
#define Set_Lo         0x20 //Select the Low Local Oscillator frequency 
#define Set_VR         0x21 //Select Vertical Polarisation (or Right circular) 
#define Set_Pos_A         0x22 //Select Satellite position A (or position C) 
#define Set_S0A          0x23 //Select Switch Option A (e.g. positions A/B) 
#define Set_Hi          0x24 //Select the High Local Oscillator frequency 
#define Set_HL         0x25 //Select Horizontal Polarisation (or Left circular) 
#define Set_Pos_B          0x26 //Select Satellite position B (or position D) 
#define Set_S0B         0x27 //Select Switch Option B (e.g. positions C/D)
#define Set_S1A         0x28 //Select switch S1 input A (deselect input B)
#define Set_S2A         0x29 //Select switch S2 input A (deselect input B)
#define Set_S3A         0x2A //Select switch S3 input A (deselect input B)
#define Set_S4A         0x2B //Select switch S4 input A (deselect input B)
#define Set_S1B         0x2C //Select switch S1 input B (deselect input A)
#define Set_S2B         0x2D //Select switch S2 input B (deselect input A) 
#define Set_S3B         0x2E //Select switch S3 input B (deselect input A)
#define Set_S4B         0x2F //Select switch S4 input B (deselect input A)
*/
#define Sleep 0x30 //Ignore all bus commands except “Awake”
#define Awake 0x31 //Respond to future bus commands normally
/*
#define Write_N0        0x38 //Write to Port group 0 (Committed switches)
#define Write_N1        0x39 //Write to Port group 1 (Uncommitted switches) 
#define Write_N2        0x3A //expansion option 
#define Write_N3        0x3B //expansion option 
#define Read_A0         0x40 //Read Analogue value A0 
#define Read_A1         0x41 //Read Analogue value A1
#define Write_A0        0x48 //Write Analogue value A0 (e.g. Skew) 
#define Write_A1        0x49 //Write Analogue value A1 
#define LO_string       0x50 //Read current frequency [Reply = BCD string] 
#define LO_now          0x51 //Read current frequency table entry number
#define LO _Lo          0x52 //Read Lo frequency table entry number
#define LO_Hi           0x53 //Read Hi frequency table entry number
#define Write_Freq          0x58 //Write channel frequency (BCD string)
#define Ch_No         0x59 //Write (receiver’s) selected channel number
*/
#define Halt 0x60 //Stop Positioner movement
//61 Reserved
//62 Reserved
#define Limits_Off 0x63 //Disable Limits
#define PosStat 0x64    //Read Positioner Status Register
//65 Reserved
#define Limit_E 0x66    //Set East Limit (& Enable recommended)
#define Limit_W 0x67    //Set West Limit (& Enable recommended)
#define Drive_East 0x68 //Drive Motor East (with optional timeout/steps)
#define Drive_West 0x69 //Drive Motor West (with optional timeout/steps)
#define Store_nn 0x6A   //Store Satellite Position & Enable Limits
#define Goto_nn 0x6B    //Drive Motor to Satellite Position nn
//6C Reserved
//6D Reserved
#define Goto_x_x 0x6E  //Drive Motor to Angular Position (°)
#define Set_Posns 0x6F //(Re-) Calculate Satellite Positions

#define MS_TO_USEC(x) (x * 1000U) // Convert milli-Seconds to micro-Seconds.
const uint16_t kStartOffset = 1;  // Usual rawbuf entry to start from.
// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
const uint16_t kMarkExcess = 50;
const uint16_t kRawBuf = 100; // Default length of raw capture buffer
const uint64_t kRepeat = UINT64_MAX;
// Default min size of reported UNKNOWN messages.
//const uint16_t kUnknownThreshold = 6;

// receiver states
const uint8_t kIdleState = 3;
const uint8_t kMarkState = 4;
const uint8_t kStopState = 6;
//const uint8_t kSpaceState = 5;

const uint16_t Bit_1_PulseWidthUs = (500);  // ||||||||____________
const uint16_t Bit_0_PulseWidthUs = (1000); // ||||||||||||||______

//const uint8_t kTolerance = (25 / 100); // default percent tolerance in measurements.
//const uint8_t kUseDefTol = 255;        // Indicate to use the class default tolerance.
const uint16_t kRawTick = 1; // Capture tick to uSec factor.
#define RAWTICK kRawTick
const uint8_t kTimeoutMs = 5; // In MilliSeconds.
#define TIMEOUT_MS kTimeoutMs // For legacy documentation.
const uint16_t kMaxTimeoutMs = kRawTick * (UINT16_MAX / MS_TO_USEC(1));
const uint16_t kStateSizeMax = 53;

// Types

typedef struct
{
  uint8_t recvpin;  // pin for diseqc1.2 data from detector
  uint8_t rcvstate; // state machine
  uint16_t timer;   // state timer, counts 50uS ticks.
  uint16_t bufsize; // max. nr. of entries in the capture buffer.
  uint16_t *rawbuf; // raw data
  uint16_t rawlen;  // counter of entries in rawbuf.
  uint8_t overflow; // Buffer overflow indicator.
  uint8_t timeout;  // Nr. of milliSeconds before we give up.
} DiseqcParams_t;

// define's

// Classes

/// Results returned from the decoder
class decode_results
{
public:
  union
  {
    struct
    {
      uint32_t data;    // Decoded data
      uint8_t command;  // Decoded command.
      uint8_t address;  // Decoded device address.
      uint8_t framming; // Decoded framming
    };
    uint8_t state[kStateSizeMax]; // Multi-byte results.
  };
  uint16_t bits;             // Number of bits in decoded value
  volatile uint16_t *rawbuf; // Raw intervals in .5 us ticks
  uint16_t rawlen;           // Number of records in rawbuf.
  bool overflow;
  bool repeat; // Is the result a repeat code?
};

/// Class for receiving Diseqc messages.
class Diseqc
{
public:
  explicit Diseqc(const uint16_t recvpin,
                  const uint16_t bufsize = kRawBuf, const uint8_t timeout = kTimeoutMs); // Constructor

  ~Diseqc(void); // Destructor

  bool decode(decode_results *results);
  bool ConvertRawTObit(uint16_t *input, uint8_t *output, uint16_t len);
  void begin(const bool pullup = false);
  void disableDiseqcIn(void);
  void resume(void);

  DiseqcParams_t *DiseqcParams_t_save;
};

#endif // DISEQCRECV_H_