#include "wificonfig.h"
#include <Arduino.h>
#include "EEPROM_ADD.h"

// Globals
factory_values default_values;

// Function's
void read_data(int address, String &data, uint8_t max_len)
{
    for (int i = address; true; i++)
    {
        if (char(EEPROM.read(i)) == '\0')
            break;
        if (i == max_len)
            break;
        data += char(EEPROM.read(i));
    }
}

void write_data(int address, String &data, uint8_t max_len)
{
    uint8_t size;
    size = data.length();
    for (int i = 0; i < address + size; i++)
    {
        if (i == max_len)
            break;
        EEPROM.write(address + i, char(data[i]));
    }
    EEPROM.write(address + size, char('\0'));
    EEPROM.commit();
}
// Start of wificonfig class -------------------

wificonfig ::wificonfig(String APSSID, String APPSK,
                        String ws_username, String ws_password, uint8_t is_ap)
{
    default_values.factory_SSID = APSSID;
    default_values.factory_PSK = APPSK;
    default_values.factory_username = ws_username;
    default_values.factory_password = ws_password;
    default_values.factory_is_ap = is_ap;

    // if RESET WIFI CONFIG
    if (EEPROM.read(RESET_WIFICONFIG_ADD) != 0xf0)
    {
        RESET();
    }
}

wificonfig::~wificonfig()
{
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

void wificonfig ::Get_IS_AP(uint8_t &input)
{
    input = EEPROM.read(is_ap_ADD);
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

bool wificonfig ::Set_IS_AP(uint8_t &input)
{
    EEPROM.write(is_ap_ADD, input);
    EEPROM.commit();
    return true;
}

void wificonfig::RESET()
{
    Set_SSID(default_values.factory_SSID);
    Set_PSK(default_values.factory_PSK);
    Set_USERNAMME(default_values.factory_username);
    Set_PASSWORD(default_values.factory_password);
    Set_IS_AP(default_values.factory_is_ap);

    EEPROM.write(RESET_WIFICONFIG_ADD, 0xf0);
    EEPROM.commit();
}
