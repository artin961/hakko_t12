#include "IronTips.h"

// Single string with all tip names in PROGMEM
const char tip_names[] PROGMEM =
  "B\0B1\0B2\0B3\0B4\0B2Z\0BC1\0BC1.5\0BC2\0BC3\0BC1Z\0BC2Z\0BC4Z\0"
  "BCF1\0BCF2\0BCF3\0BCF4\0BCF1Z\0BCF2Z\0BCF3Z\0BCM2\0BCM3\0"
  "BL\0BZ\0C1\0C2\0C3\0C4\0D08\0D12\0D16\0D24\0D52\0D4\0"
  "DL12\0DL32\0DL52\0D12Z\0D24Z\0D4Z\0I\0IL\0ILS\0J02\0JL02\0JS02\0"
  "K\0KF\0KL\0KR\0KFZ\0KRZ\0KU\0WB2\0WD08\0WD12\0WD16\0WD52\0WI\0"
  "N1-06\0N1-08\0N1-10\0N1-13\0N1-16\0N1-20\0N1-23\0N1-L1\0"
  "1001\01002\01003\01004\01005\01006\01007\01008\01009\01010\0"
  "1201\01202\01203\01204\01205\01206\01207\01208\01209\0"
  "1401\01402\01403\01404\0"
  "J741\0J903\0";

// Calculate the total number of tips by counting how many null-terminated strings we have in `tip_names`
uint16_t TIPS::tipsLoaded(void) {
    uint16_t count = 0;
    const char *ptr = tip_names;
    
    while (pgm_read_byte(ptr) != '\0') {
        count++;
        ptr += strlen_P(ptr) + 1;  // Move to the next tip
    }
    
    return count;
}

// Retrieve the name of a tip at the given index
bool TIPS::name(char tip_n[tip_name_sz], uint8_t index) {
    const char *ptr = tip_names;

    // Locate the desired tip by skipping past previous names
    for (uint8_t i = 0; i < index; i++) {
        if (pgm_read_byte(ptr) == '\0') return false;  // Index out of bounds
        ptr += strlen_P(ptr) + 1;
    }

    // Copy the tip name into tip_n
    strncpy_P(tip_n, ptr, tip_name_sz - 1);
    tip_n[tip_name_sz - 1] = '\0';  // Ensure null termination
    return true;
}

// Search for the index of the tip with the given name
int8_t TIPS::index(const char name[tip_name_sz]) {
    const char *ptr = tip_names;
    int8_t i = 0;

    while (pgm_read_byte(ptr) != '\0') {
        if (strncmp_P(name, ptr, tip_name_sz) == 0) {
            return i;  // Found match
        }
        ptr += strlen_P(ptr) + 1;
        i++;
    }
    
    return -1;  // Not found
}
