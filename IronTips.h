#ifndef _IRON_TIPS_H
#define _IRON_TIPS_H

#include <Arduino.h>
#include <string.h>


// The length of the tip name
#define tip_name_sz 5

/*
   The custip TIP structure, 12 bytes
   The lower EEPROM area reserved for storing up to MAX_CUSTOM_TIPS configuration data.
   When a tip is activated, new structure is allocated for it in the lower EEPROM Area.
   The Active tips are stored consecutively in this area. Tip index is the number of the tip
   in the custom area.
   When the tip is disabled, the tip configuration data is kept in the EEPROM area
   until space is required for another tip data.
*/
typedef struct s_tip TIP;
struct s_tip {
  char        tip_name[tip_name_sz];      // The TIP name
  uint32_t    calibration_data;           // The temperature calibration data for soldering tips (3 reference points: 200, 300, 400 Centigrades)
  uint8_t     mask;                       // The bit mask: TIP_ACTIVE + TIP_CALIBRATED
  int8_t      ambient;                    // The ambient temperature in Celsius when the tip was calibrated
  uint8_t     crc;                        // CRC checksum
};

typedef enum tip_status { TIP_ACTIVE = 1, TIP_CALIBRATED = 2 } TIP_STATUS;

class TIPS {
public:
    TIPS() {}
    uint16_t    tipsLoaded(void);
    bool        name(char tip_n[tip_name_sz], uint8_t index);
    int8_t      index(const char name[tip_name_sz]);
};

#endif
