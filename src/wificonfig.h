#ifndef WIFICONFIG_H_
#define WIFICONFIG_H_

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <stddef.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>

// Constants

// define's

//type's

typedef struct
{
    String factory_SSID;
    String factory_PSK;
    String factory_username;
    String factory_password;
    uint8_t factory_is_ap;
} factory_values;

//Function's
void read_data(int address, String &data, uint8_t max_len = 14);
void write_data(int address, String &data, uint8_t max_len = 14);

//class's

/// Class for wificonfig.
class wificonfig
{
public:
    explicit wificonfig(String APSSID = "Positioner", String APPSK = "12345678",
                        String ws_username = "admin", String ws_password = "admin",
                         uint8_t is_ap = 1); // Constructor

    ~wificonfig(void); // Destructor
    void Get_SSID(String &input);
    void Get_PSK(String &input);
    void Get_USERNAMME(String &input);
    void Get_PASSWORD(String &input);
    void Get_IS_AP(uint8_t &input);

    bool Set_SSID(String &input);
    bool Set_PSK(String &input);
    bool Set_USERNAMME(String &input);
    bool Set_PASSWORD(String &input);
    bool Set_IS_AP(uint8_t &input);

    void RESET();
};
#endif