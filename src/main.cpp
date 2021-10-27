#ifndef UNIT_TEST
#include <Arduino.h>
#endif

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>

#include "EEPROM_ADD.h"
#include "Diseqc.h"
#include "Positioner.h"
#include "wificonfig.h"

//html page
#include "index.h"
#include "login.h"
#include "wificonfigpage.h"

#define AccessPoint

//eeprom
ManageEEPROM MyEEPROM(512);

//WIFI
String ssid = "";//Positioner
String ssid_password = "";//12345678
String ws_username = "";//admin
String ws_password = "";//admin

ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer server(80);
wificonfig wifiSetting;

//LOG
String Poslog = "";
uint8_t logCount = 20;

//DISEQC AND POSITIONER
uint16_t RECV_PIN = D2;   //GPIO4
uint16_t SENSOR_PIN = D1; //GPIO5
uint16_t EAST_PIN = D3;   //GPIO0
uint16_t WEST_PIN = D4;   //GPIO2
enum Error PositionerError = Error::none;
decode_results results;

Diseqc diseqc(RECV_PIN, 1024, kTimeoutMs);
Positioner positioner(SENSOR_PIN, EAST_PIN, WEST_PIN);

//Function's

void MOV()
{
  String state = server.arg("CMD");
  if (state == "1")
  {
    positioner.MoveToEast();
    state = "EAST";
  }
  else if (state == "2")
  {
    positioner.MoveToWest();
    state = "WEST";
  }
  else if (state == "0")
  {
    positioner.StopMovement();
    state = "HALT";
  }

  server.send(200, "text/plane", state);
}

void GET_STATUS()
{
  uint8_t state = positioner.GetPosState();
  //read from eeprom
  uint16_t cnt = positioner.GetStatus();
  uint8_t currentSAT = positioner.GetCurrentSat();
  uint16_t westLimit = positioner.GetWestLimit();
  uint16_t EastLimit = positioner.GetEastLimit();
  uint8_t limit = positioner.GetLimit();

  String sensor_value = String(cnt);

  sensor_value += "`" + Poslog + "`" + currentSAT + "`" + westLimit + "`" + EastLimit +
                  "`" + limit + "`" + PositionerError + "`" + state;

  server.send(200, "text/plane", sensor_value);
  Poslog = "0";
}

void handleRoot()
{
  String s = FPSTR(webpage);

  server.send(200, "text/html", s);
}

void handleConfig()
{
  if (server.method() == HTTP_GET)
  {
    String s = FPSTR(wifiConfigPage);
    server.send(200, "text/html", s);
  }
  else if (server.method() == HTTP_POST)
  {
    String _ssid = server.arg("ssid");
    String _psk = server.arg("psk");

    wifiSetting.Set_SSID(_ssid);
    wifiSetting.Set_PSK(_psk);

    String _username = server.arg("username");
    String _password = server.arg("password");
    //if (_username.length() > 0 && _password.length() > 0)
    {
      wifiSetting.Set_USERNAMME(_username);
      wifiSetting.Set_PASSWORD(_password);
    }

    server.send(200, "text/plane", "Update Success! Rebooting...");
    delay(1000);
    //ESP.reset();
    ESP.restart();
  }
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void)
{
  wifiSetting.Get_SSID(ssid);
  wifiSetting.Get_PSK(ssid_password);
  wifiSetting.Get_USERNAMME(ws_username);
  wifiSetting.Get_PASSWORD(ws_password);

  httpUpdater.setup(&server, "/update", ws_username, ws_password);

  diseqc.begin();

  delay(500);

#ifndef AccessPoint
  WiFi.begin(ssid, password);
  // Wait for connection
  //Serial.printf("Connecting to %s ", ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //Serial.print(".");
  }
/*Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());*/
#else
  WiFi.softAP(ssid.c_str(), ssid_password.c_str(), 8);
  //IPAddress myIP = WiFi.softAPIP();
#endif
  server.on("/", []
            {
              if (!server.authenticate(ws_username.c_str(), ws_password.c_str()))
                server.requestAuthentication(DIGEST_AUTH);
              handleRoot();
            });
  server.on("/config", []
            {
              if (!server.authenticate(ws_username.c_str(), ws_password.c_str()))
                server.requestAuthentication(DIGEST_AUTH);
              handleConfig();
            });
  server.on("/MOV", MOV);
  server.on("/GET_STATUS", GET_STATUS);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop(void)
{
  //running
  if (positioner.GetPosState() != haltState)
  {
    /******************/
    mycounter();
    /******************/
    //error check
    if (positioner.CheckMotorTimout())
    {
      PositionerError = Error::SensorConnectorORHardwareLimit;
    }
    else
    {
      PositionerError = Error::none;
    }
  }

  server.handleClient();

  //store current SRAM status in eeprom if not match.
  if (positioner.GetStatusFromSRAM() != positioner.GetStatus())
  {
    positioner.SetStatus(positioner.GetStatusFromSRAM());
  }

  //decode diseqc message.
  if (diseqc.decode(&results))
  {
    char str[20];
    sprintf(str, "%02X %02X %02X %06X ", results.framming, results.address, results.command, results.data);
    Poslog = (String)str;

    if ((results.framming == Master_First_transmission_No_reply ||
         results.framming == Master_Repeated_First_transmission_No_reply) &&
        (results.address == Any_Positioner ||
         results.address == Polar_Azimuth_Positioner))
    {
      switch (results.command)
      {
      case Reset: //Reset DiSEqCä microcontroller
        ESP.restart();
        Poslog += "restart";
        break;

      case Clr_Reset: // Clear the “Reset” flag
        positioner.ClearFlag();
        Poslog += "ClearFlag";
        break;

      case Standby: // Stand by :Switch peripheral power supply off
        //ESP.deepSleep(0); //deep sleep mode until RESET pin is connected to a LOW signal
        Poslog += "Standby";
        break;

      case Power_on: // Power on :Switch peripheral power supply on
        Poslog += "Poweron";
        break;

      case Halt: //Stop Positioner movement
        positioner.StopMovement();
        Poslog += "StopMovement";
        break;

      case Limits_Off: //Disable Limits
        positioner.DisableLimits();
        Poslog += "DisableLimits";
        break;

      case Limit_E: //Set East Limit (& Enable recommended)
        positioner.SetEastLimit(positioner.GetStatusFromSRAM());
        Poslog += "SetEastLimit";
        positioner.EnableLimits();
        break;

      case Limit_W: //Set West Limit (& Enable recommended)
        positioner.SetWestLimit(positioner.GetStatusFromSRAM());
        Poslog += "SetWestLimit";
        positioner.EnableLimits();
        break;

      case Drive_East: //Drive Motor East (with optional timeout/steps)
        positioner.MoveToEast();
        Poslog += "MoveToEast";
        break;

      case Drive_West: //Drive Motor West (with optional timeout/steps)
        positioner.MoveToWest();
        Poslog += "MoveToWest";
        break;

      case Store_nn: //Store Satellite Position & Enable Limits
        if (results.data == 0)
        {
          positioner.EnableLimits();
          Poslog += "EnableLimits";
        }
        else
        {
          uint16_t status = positioner.GetStatusFromSRAM();
          positioner.StoreSatellite(results.data, status);
          positioner.SetCurrentSat(results.data);
          Poslog += "Store" + (String)results.data;
        }
        break;

      case Goto_nn: //Drive Motor to Satellite Position nn
        positioner.MoveToSatellite(results.data);
        Poslog += "Goto" + (String)results.data;
        break;

      case Set_Posns: //(Re-) Calculate Satellite Positions
        positioner.ReCalculatePositions(results.data);
        Poslog += "ReCalculate" + (String)results.data;
        break;

      default:
        Poslog += "NotSupportCMD";
        break;
      }
    }

    diseqc.resume();
  }
}
