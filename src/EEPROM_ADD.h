#ifndef EEPROM_ADD_
#define EEPROM_ADD_

#include <EEPROM.h>
// eeprom address for positioner value

#define LIMIT_ADD 0       // 1 BYTE 0
#define EAST_LIMIT_ADD 1  // 2 BYTE (1-3)
#define WEST_LIMIT_ADD 3  // 2 BYTE (3-5)
#define STATUS_ADD 5      // 2 BYTE (5-7)
#define CURRENT_SAT_ADD 7 // 1 BYTE 7
// 8-9 RESERVE for default_factory? (first config)
#define RESET_FACTORY_ADD 8                      // 1 BYTE 8
#define SAT_STATUS_ADD(SATnn) (10 + (SATnn * 2)) // 99 Satellite x 2 BYTE (10-206)

// eeprom address for wifi config value

#define RESET_WIFICONFIG_ADD 210                 // 1
#define APSSID_ADD (RESET_WIFICONFIG_ADD + (1))  // 15
#define APPSK_ADD (APSSID_ADD + (15))            // 15
#define ws_username_ADD (APPSK_ADD + (15))       // 15
#define ws_password_ADD (ws_username_ADD + (15)) // 15
#define is_ap_ADD (ws_password_ADD + (15))       // 1

// Function

/// class
class ManageEEPROM
{
public:
    explicit ManageEEPROM(int size = 512)
    {
        EEPROM.begin(512);
    } // Constructor

    ~ManageEEPROM(void)
    {

    } // Destructor
};
#endif