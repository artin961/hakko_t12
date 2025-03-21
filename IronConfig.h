#ifndef _IRON_CONFIG_H
#define _IRON_CONFIG_H
#include <Arduino.h>
#include <EEPROM.h>
#include "IronTips.h"
#include "Config.h"
#include "vars.h"
//#define DEBUGEEE
/*
   First 504 bytes of EEPROM are allocated to store the calibration tip data (see iron_tips.h)
   The EEPROM space starting from 512 is used to store the controller configuration data
   in the following format. Each record requires 13 bytes, and it will rounded to 16 bytes.

   uint32_t ID                                  The ID of the record. Each time incremented by 1
   struct cfg                                   config data, 8 bytes
   uint8_t CRC                                  the checksum
*/
// Configuration structure for storing preset values and other configurations
struct cfg {
  uint16_t    temp;           // Preset temperature (Celsius or Fahrenheit)
  uint8_t     tip;            // Current tip index [0 - MAX_CUSTOM_TIPS]
  uint16_t    low_temp;       // Low power temperature (C) or 0 if tilt sensor is disabled
  uint8_t     low_to;         // Low power timeout (seconds)
  uint8_t     off_timeout;    // Auto switch-off timeout in minutes [0 - 30]
  uint8_t     bit_mask;       // Configuration bit mask
};

// Configuration bit map options
typedef enum {
  CFG_CELSIUS = 1,
  CFG_BUZZER = 2,
  CFG_SWITCH = 4,
  CFG_THERM = 8
} CFG_BIT_MASK;

// Configuration class for EEPROM management and settings retrieval
class CONFIG {
  public:
    CONFIG(uint8_t max_custom_tips);
    void    init();
    bool    load(void);
    void    getConfig(struct cfg &Cfg);
    void    updateConfig(struct cfg &Cfg);
    bool    save(void);
    bool    saveConfig(struct cfg &Cfg);
  protected:
    struct cfg Config;
  private:
    bool        readRecord(uint16_t addr, uint32_t &rec_ID);
    bool        can_write;      // Flag for save permission
    uint16_t    start_addr = 0; // Starting EEPROM address for configuration records
    uint16_t    r_addr;         // Address for reading the correct record in EEPROM
    uint16_t    w_addr;         // Address to write new record to EEPROM
    uint16_t    e_length;       // EEPROM size depending on Arduino model
    uint32_t    next_rec_ID;    // ID for next record to be written
    uint8_t     record_size;    // Size of each record (13 bytes)
};

// IRON configuration class that extends both CONFIG and TIPS
class IRON_CFG : public CONFIG, public TIPS {
  public:
    IRON_CFG(uint8_t max_custom_tips) : CONFIG(max_custom_tips) { }
    void        init(void);
    bool        isCelsius(void)             {
      return Config.bit_mask & CFG_CELSIUS;
    }
    bool        isBuzzer(void)              {
      return Config.bit_mask & CFG_BUZZER;
    }
    uint16_t    tempPresetHuman(void)       {
      return Config.temp;
    }
    uint8_t     tipIndex(void)              {
      return tip_index;
    }
    bool        isCalibrated(void)          {
      return tip_mask & TIP_CALIBRATED;
    }
    uint8_t     getOffTimeout(void)         {
      return Config.off_timeout;
    }
    uint8_t     getLowTemp(void)            {
      return Config.low_temp;
    }
    uint8_t     getLowTimeout(void)         {
      return Config.low_to;
    }
    char*       tipName(void)               {
      return tip_name;
    }
    bool        isReedType(void)            {
      return Config.bit_mask & CFG_SWITCH;
    }
    bool        isAmbientSensor(void)       {
      return Config.bit_mask & CFG_THERM;
    }
    uint16_t    humanToTemp(uint16_t temp, int16_t ambient);
    uint16_t    tempToHuman(uint16_t temp, int16_t ambient);
    uint8_t     selectTip(uint8_t index);
    bool        savePresetTempHuman(uint16_t temp);
    bool        savePresetTemp(uint16_t temp);
    void        applyCalibration(uint16_t tip[3]);
    void        saveConfig(uint8_t off, bool cels, bool buzzer, bool ambient, uint16_t low_temp, uint8_t low_to, bool reed);
    void        getCalibrationData(uint16_t tip[3]);
    void        saveCalibrationData(uint16_t tip[3], int8_t ambient);
    uint8_t     nextTip(uint8_t index, bool forward = true);
    bool        isTipActive(uint8_t global_index);
    bool        toggleTipActivation(uint8_t global_index);

  private:
    bool        save(void);
    bool        checkTipCRC(TIP& tip_data, bool write = false);
    bool        loadTipData(TIP* tip_data, uint8_t index);
    bool        validateTip(TIP& tip_data);
    void        setDefaults(void);         // Set default parameter values if failed to load data
    void        unpackTipCalibration(uint16_t tip[3], uint32_t cd);
    int8_t      calibratedTipIndex(const char name[tip_name_sz]);
    int8_t      emptyTipSlot(void);
    void        removeTipDuplication(void);
    char        tip_name[tip_name_sz + 1]; // Tip name buffer
    struct cfg  previous_cfg;              // Stored config for comparison
    uint16_t    t_tip[3] = {0};            // Current tip calibration
    uint8_t     tip_index = 0;             // Current tip index
    uint8_t     tip_mask = 0;              // Current tip status
    int8_t      tip_ambient = ambient_tempC; // Ambient temperature during calibration
    const uint16_t def_tip[3] = {CONFIG_INERNAL_READING_200, CONFIG_INERNAL_READING_300, CONFIG_INERNAL_READING_400}; // Default calibration values
};
#endif
