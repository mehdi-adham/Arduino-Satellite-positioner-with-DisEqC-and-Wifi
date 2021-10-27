#include "Positioner.h"
#include <EEPROM.h>
extern "C"
{
#include <gpio.h>
#include <user_interface.h>
}
#include <Arduino.h>
#include "EEPROM_ADD.h"

// Globals

#define USE_IRAM_ATTR ICACHE_RAM_ATTR

#define none 65535

volatile PositionerParams_t posparams;
volatile uint32_t startTime;

// one NOP takes 1/80000000 of a second if ARM is running at 80MHz
//#define NOP __asm__ __volatile__("nop") // 12ns delay
/*for (int i = 0; i < 50; i++)
    NOP; // small delay gives the pin time to change
  if (digitalRead(posparams.sensorpin) == LOW)
    return;*/
/*
static void USE_IRAM_ATTR counter_intr()
{
    //Reject count in halt
    if (posparams.posstate == haltState)
        return;

    uint32_t _now = micros();
    static uint32_t _start = 0;
    uint16_t _len = cntrlen;
    uint32_t def = 0;

    if (cntrlen >= 256)
        _len = cntrlen = 0;

    if (_now < _start)
        def = (UINT32_MAX - _start + _now);
    else
        def = (_now - _start);

    _start = _now;

    //Reject
    if (def < 35000)
        return;

    if (digitalRead(posparams.sensorpin) == HIGH)
        return;

    //store speed
    cntrbuf[_len] = def;
    cntrlen++;

    //counter up/down
    if (posparams.posstate == EastState)
        posparams.status--;
    else if (posparams.posstate == WestState)
        posparams.status++;

    //Check Going to satellite nn
    if (posparams.GotoSat != none)
    {
        if (posparams.status == posparams.GotoStatus) // Check satellite nn reached
        {
            posparams.posstate = haltState;
            posparams.currentSat = posparams.GotoSat;
            posparams.GotoSat = none;
        }
    }

    //check for limit
    if (posparams.ISLimit)
    {
        if ((posparams.status == posparams.EastLimit) || // when arrive to limit
            posparams.status == posparams.WestLimit)
            posparams.posstate = haltState;
    }

    //check for stop movemen
    if (posparams.posstate == haltState)
    {
        Stop();
        return;
    }
}
*/

//Function's
void mycounter()
{
    static bool preval;
    if (digitalRead(posparams.sensorpin) == HIGH)
        preval = false;
    if (digitalRead(posparams.sensorpin) == LOW && preval == false)
    {
        delay(40);
        preval = true;
        //counter up/down
        if (posparams.posstate == EastState)
            posparams.status--;
        else if (posparams.posstate == WestState)
            posparams.status++;

        //Check Going to satellite nn
        if (posparams.GotoSat != none)
        {
            if (posparams.status == posparams.GotoStatus) // Check satellite nn reached
            {
                posparams.posstate = haltState;
                posparams.currentSat = posparams.GotoSat;
                posparams.GotoSat = none;
            }
        }

        //check for limit
        if (posparams.ISLimit)
        {
            if ((posparams.status == posparams.EastLimit) || // when arrive to limit
                posparams.status == posparams.WestLimit)
                posparams.posstate = haltState;
        }

        //check for stop movemen
        if (posparams.posstate == haltState)
            Stop();
    }
}

void Stop()
{
    digitalWrite(posparams.eastpin, HIGH);
    digitalWrite(posparams.westpin, HIGH);
}

// Start of Satellite Positioner class -------------------

Positioner ::Positioner(const uint16_t sensorpin, const uint16_t eastpin,
                        const uint16_t westpin)
{
    posparams.sensorpin = sensorpin;
    posparams.eastpin = eastpin;
    posparams.westpin = westpin;

    pinMode(posparams.eastpin, OUTPUT);
    pinMode(posparams.westpin, OUTPUT);

    enableSensorIn();

    Stop();

    if (EEPROM.read(RESET_FACTORY_ADD) != 0xf0)
    {
        ResetFactory();
        // ESP.restart();
    }

    posparams.GotoSat = none;
    posparams.currentSat = GetCurrentSat();
    posparams.EastLimit = GetEastLimit();
    posparams.WestLimit = GetWestLimit();
    posparams.status = GetStatus();
    posparams.posstate = haltState;
    posparams.ISLimit = GetLimit();
}

Positioner::~Positioner()
{
}

bool Positioner::CheckMotorTimout()
{
    static uint16_t prevalue = 0;
    uint32_t def = 0;

    if (prevalue != posparams.status)
    {
        prevalue = posparams.status;
        startTime = micros();
    }
    else
    {
        uint32_t now = micros();
        if (now < startTime)
            def = (UINT32_MAX - startTime + now);
        else
            def = (now - startTime);

        //MOTOR TIMEOUT MOTOR_TIMEOUT(5) S
        if ((def / 1000000) > MOTOR_TIMEOUT)
        {
            StopMovement();
            return true;
        }
    }
    return false;
}

void Positioner::enableSensorIn(const bool pullup)
{
    if (pullup)
        pinMode(posparams.sensorpin, INPUT_PULLUP);
    else
        pinMode(posparams.sensorpin, INPUT);
    // اینتراپت پین سنسور موتور
    //attachInterrupt(posparams.sensorpin, counter_intr, FALLING);
}

void Positioner::disableSensorIn()
{
    detachInterrupt(posparams.sensorpin);
}

void Positioner::MoveToEast()
{
    StopMovement();

    digitalWrite(posparams.eastpin, LOW);
    digitalWrite(posparams.westpin, HIGH);

    SetPosState(EastState);
    startTime = micros();
}

void Positioner::MoveToWest()
{
    StopMovement();

    digitalWrite(posparams.eastpin, HIGH);
    digitalWrite(posparams.westpin, LOW);

    SetPosState(WestState);
    startTime = micros();
}

void Positioner::StopMovement()
{
    SetPosState(haltState);
    Stop();
}

void Positioner::DisableLimits()
{
    posparams.ISLimit = false;
    EEPROM.write(LIMIT_ADD, posparams.ISLimit);
    EEPROM.commit();
}

void Positioner::EnableLimits()
{
    posparams.ISLimit = true;
    EEPROM.write(LIMIT_ADD, posparams.ISLimit);
    EEPROM.commit();
}

void Positioner::StoreSatellite(uint8_t SATnn, uint16_t status)
{
    EEPROM.write(SAT_STATUS_ADD(SATnn), (uint8_t)status);
    EEPROM.write(SAT_STATUS_ADD(SATnn) + 1, (uint8_t)(status >> 8));
    EEPROM.commit();
}

void Positioner::MoveToSatellite(uint8_t SATnn)
{
    posparams.GotoSat = SATnn;
    posparams.GotoStatus = GetStatusSatnn(posparams.GotoSat);

    /*just change*/
    if (posparams.GotoStatus == none)
        return;

    if (posparams.status < posparams.GotoStatus)
        MoveToWest();
    else if (posparams.status > posparams.GotoStatus)
        MoveToEast();
}

void Positioner::ReCalculatePositions(uint16_t data)
{
    uint8_t CurrentSat = GetCurrentSat();
    uint16_t CurrentSatStatus = GetStatusSatnn(CurrentSat);
    int dif = posparams.status - CurrentSatStatus;
    // example1: 1300 - 1200 = 100
    // example2: 1200 - 1300 = -100
    for (uint8_t SATnn = 1; SATnn < 100; SATnn++)
    {
        CurrentSatStatus = GetStatusSatnn(SATnn);
        if (CurrentSatStatus == none)
            continue;

        StoreSatellite(SATnn, CurrentSatStatus + dif);
    }
}

void Positioner::ResetFactory()
{
    EEPROM.write(RESET_FACTORY_ADD, 0xf0);
    EEPROM.commit();
    SetPosState(haltState);
    SetEastLimit(0);
    SetWestLimit(0);
    DisableLimits();
    SetStatus(ZeroStatus);
    StoreSatellite(0, ZeroStatus);
    for (uint8_t SATnn = 1; SATnn < 100; SATnn++)
        StoreSatellite(SATnn, none);
    SetCurrentSat(0);
}

void Positioner::ClearFlag()
{
    /**/
}

// Getter and Setter

void Positioner::SetPosState(uint16_t PosState)
{
    posparams.posstate = PosState;
}

uint8_t Positioner::GetPosState()
{
    return posparams.posstate;
}

uint16_t Positioner::GetStatusFromSRAM()
{
    return posparams.status;
}

uint16_t Positioner::GetStatus()
{
    return ((0x00ff & EEPROM.read(STATUS_ADD)) +
            (0xff00 & (EEPROM.read(STATUS_ADD + 1) << 8)));
}

uint16_t Positioner ::GetStatusSatnn(uint8_t SATnn)
{
    return ((0x00ff & EEPROM.read(SAT_STATUS_ADD(SATnn))) +
            (0xff00 & (EEPROM.read(SAT_STATUS_ADD(SATnn) + 1) << 8)));
}

void Positioner::SetEastLimit(uint16_t status)
{
    posparams.EastLimit = status;
    EEPROM.write(EAST_LIMIT_ADD, (uint8_t)posparams.EastLimit);
    EEPROM.write(EAST_LIMIT_ADD + 1, (uint8_t)(posparams.EastLimit >> 8));
    EEPROM.commit();
}

uint16_t Positioner::GetEastLimit()
{
    posparams.EastLimit = ((0x00ff & EEPROM.read(EAST_LIMIT_ADD)) +
                           (0xff00 & (EEPROM.read(EAST_LIMIT_ADD + 1) << 8)));
    return posparams.EastLimit;
}

void Positioner::SetWestLimit(uint16_t status)
{
    posparams.WestLimit = status;
    EEPROM.write(WEST_LIMIT_ADD, (uint8_t)posparams.WestLimit);
    EEPROM.write(WEST_LIMIT_ADD + 1, (uint8_t)(posparams.WestLimit >> 8));
    EEPROM.commit();
}

uint16_t Positioner::GetWestLimit()
{
    posparams.WestLimit = ((0x00ff & EEPROM.read(WEST_LIMIT_ADD)) +
                           (0xff00 & (EEPROM.read(WEST_LIMIT_ADD + 1) << 8)));
    return posparams.WestLimit;
}

uint8_t Positioner::GetCurrentSat()
{
    posparams.currentSat = EEPROM.read(CURRENT_SAT_ADD);
    return posparams.currentSat;
}

void Positioner::SetCurrentSat(uint8_t Sat)
{
    posparams.currentSat = Sat;
    EEPROM.write(CURRENT_SAT_ADD, (uint8_t)posparams.currentSat);
    EEPROM.commit();
}

void Positioner::SetStatus(uint16_t status)
{
    posparams.status = status;
    EEPROM.write(STATUS_ADD, (uint8_t)posparams.status);
    EEPROM.write(STATUS_ADD + 1, (uint8_t)(posparams.status >> 8));
    EEPROM.commit();
}

uint8_t Positioner::GetLimit()
{
    posparams.ISLimit = (EEPROM.read(LIMIT_ADD));
    return posparams.ISLimit;
}
