#ifndef EEPROM_ADD_
#define EEPROM_ADD_

#include <EEPROM.h>
//eeprom address for positioner value

#define LIMIT_ADD 0       // 1 BYTE 0
#define EAST_LIMIT_ADD 1  // 2 BYTE (1-3)
#define WEST_LIMIT_ADD 3  // 2 BYTE (3-5)
#define STATUS_ADD 5      // 2 BYTE (5-7)
#define CURRENT_SAT_ADD 7 //1 BYTE 7
//8-9 RESERVE for default_factory? (first config)
#define RESET_FACTORY_ADD 8                      //1 BYTE 8
#define SAT_STATUS_ADD(SATnn) (10 + (SATnn * 2)) // 99 Satellite x 2 BYTE (10-206)

//eeprom address for wifi config value

#define RESET_WIFICONFIG_ADD 207
#define APSSID_ADD 208
#define APPSK_ADD (208 + (1 * 14))
#define ws_username_ADD (208 + (2 * 14))
#define ws_password_ADD (208 + (3 * 14))

//Function

///class
class ManageEEPROM
{
public:
    explicit ManageEEPROM(int size = 512)
    {
        EEPROM.begin(512);
    } // Constructor

    ~ManageEEPROM(void){
        
    } // Destructor
};
#endif