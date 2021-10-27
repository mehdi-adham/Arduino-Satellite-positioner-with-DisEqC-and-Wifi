#include "wificonfig.h"
#include <Arduino.h>
#include "EEPROM_ADD.h"

// Globals
factory_values default_values;

//Function's
/*
void writeString(char add, char *data, uint8_t max_len = 16)
{
    int _size = sizeof(data) / sizeof(char); //data.length();
    for (int i = 0; i < _size; i++)
    {
        EEPROM.write(add + i, data[i]);
    }
    EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
    EEPROM.commit();
}

void read_String(char add, char *data, uint8_t max_len = 16)
{
    // char data[100]; //Max 100 Bytes
    int len = 0;
    unsigned char k;
    k = EEPROM.read(add);
    while (k != '\0' && len < max_len) //Read until null character
    {
        k = EEPROM.read(add + len);
        data[len] = k;
        len++;
    }
    data[len] = '\0';
    //return String(data);
}
*/

void read_data(uint8_t add, String &data, uint8_t max_len = 14)
{
    for (uint8_t i = add; true; i++)
    {
        if (char(EEPROM.read(i)) == '\0')
            break;
        if (i == max_len)
            break;
        data += char(EEPROM.read(i));
    }
}

void write_data(uint8_t add, String &data, uint8_t max_len = 14)
{
    uint8_t size;
    size = data.length();
    for (uint8_t i = 0; i < add + size; i++)
    {
        EEPROM.write(add + i, char(data[i]));
    }
    EEPROM.write(add + size, char('\0'));
    EEPROM.commit();
}

// Start of wificonfig class -------------------

wificonfig ::wificonfig(String APSSID, String APPSK,
                        String ws_username, String ws_password)
{
    default_values.factory_SSID = APSSID;
    default_values.factory_PSK = APPSK;
    default_values.factory_username = ws_username;
    default_values.factory_password = ws_password;

    //if RESET WIFI CONFIG
    if (EEPROM.read(RESET_WIFICONFIG_ADD) != 0xf0)
    {
        RESET();
    }
}

wificonfig::~wificonfig()
{
}

bool wificonfig::Change_AP_params(APParams *APParams)
{
    EEPROM.put(APSSID_ADD, APParams->SSID);
    EEPROM.put(APPSK_ADD, APParams->PSK);
    return true;
}

bool wificonfig::Change_ws_params(wsParams *wsParams)
{
    EEPROM.put(ws_username_ADD, wsParams->username);
    EEPROM.put(ws_password_ADD, wsParams->password);
    return true;
}

void wificonfig ::Get_AP_params(APParams *APParams)
{
    EEPROM.get(APSSID_ADD, APParams->SSID);
    EEPROM.get(APPSK_ADD, APParams->PSK);
}

void wificonfig ::Get_ws_params(wsParams *wsParams)
{
    EEPROM.get(ws_username_ADD, wsParams->username);
    EEPROM.get(ws_password_ADD, wsParams->password);
}

// getter & setter

void wificonfig ::Get_SSID(String &input)
{
    read_data(APSSID_ADD, input);
}

void wificonfig ::Get_PSK(String &input)
{
    read_data(APPSK_ADD, input);
}

void wificonfig ::Get_USERNAMME(String &input)
{
    read_data(ws_username_ADD, input);
}

void wificonfig ::Get_PASSWORD(String &input)
{
    read_data(ws_password_ADD, input);
}

bool wificonfig ::Set_SSID(String &input)
{
    write_data(APSSID_ADD, input);
    return true;
}

bool wificonfig ::Set_PSK(String &input)
{
    write_data(APPSK_ADD, input);
    return true;
}

bool wificonfig ::Set_USERNAMME(String &input)
{
    write_data(ws_username_ADD, input);
    return true;
}

bool wificonfig ::Set_PASSWORD(String &input)
{
    write_data(ws_password_ADD, input);
    return true;
}

void wificonfig::RESET()
{
    Set_SSID(default_values.factory_SSID);
    Set_PSK(default_values.factory_PSK);
    Set_USERNAMME(default_values.factory_username);
    Set_PASSWORD(default_values.factory_password);

    EEPROM.write(RESET_WIFICONFIG_ADD, 0xf0);
    EEPROM.commit();
}