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

//Access Point parameter's
typedef struct
{
    const char *SSID;
    const char *PSK;
} APParams;

//web server parameter's
typedef struct
{
    const char *username;
    const char *password;
} wsParams;

typedef struct
{
    String factory_SSID;
    String factory_PSK;
    String factory_username;
    String factory_password;
} factory_values;

//Function's
void writeString(char add, String data, uint8_t max_len = 32);
String read_String(char add, uint8_t max_len = 32);

//class's

/// Class for wificonfig.
class wificonfig
{
public:
    explicit wificonfig(String APSSID = "Positioner", String APPSK = "12345678",
                        String ws_username = "admin", String ws_password = "admin"); // Constructor

    ~wificonfig(void); // Destructor

    bool Change_AP_params(APParams *APParams);
    bool Change_ws_params(wsParams *wsParams);
    void Get_AP_params(APParams *APParams);
    void Get_ws_params(wsParams *wsParams);
    void RESET();
    void Get_SSID(String &input);
    void Get_PSK(String &input);
    void Get_USERNAMME(String &input);
    void Get_PASSWORD(String &input);
    bool Set_SSID(String &input);
    bool Set_PSK(String &input);
    bool Set_USERNAMME(String &input);
    bool Set_PASSWORD(String &input);
};
#endif