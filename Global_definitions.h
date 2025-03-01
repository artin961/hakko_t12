// FIXED HEATER CONFIG
#define HEATER_DDR DDRB
#define HEATER_PORT PORTB
#define HEATER_BITMASK 0b00000100


// FIXED ROTARY ENCODER CONFIG
#define ENCODER_DDR DDRD
#define ENCODER_PORT PORTD
#define ENCODER_PIN PIND
#define ENCODER_DATA_BITMASK 0b00010000
#define ENCODER_CLOCK_BITMASK 0b00001000
#define ENCODER_BUTTON_BITMASK 0b00000100

// FIXED ROTARY ENCODER CONFIG
#define BUZZER_DDR DDRB
#define BUZZER_PORT PORTB
#define BUZZER_BITMASK 0b00001000


#define NTC_RESISTANCE 10000  //10k
#define NTC_AMBIENT_TEMP 25   //REFERENCE FOR THE RESISTANCE
#define NTC_BETA 3977         ///NTC BETA VALUE
#define NTC_PULLUP 10000      //10k PULLUP RESISTOR




#define BUZZER_PIN 11
#define probePIN A1           // Thermometer pin from soldering IRON
#define checkPIN A0           // Iron current check pin
#define termisPIN A2          // The thermistor pin to check ambient temperature
#define tiltswPIN 17          // The tilt/reed switch pin
#define lcdled_PIN 9          // The simple buzzer to make a noise
#define ledPIN 18             // The simple buzzer to make a noise




////TONES
#define CHIRP_1KHZ 500  ///TO GENERATE 1KHZ TONE USING CHIRP FUNCTION
#define CHIRP_2KHZ 250  ///TO GENERATE 2KHZ TONE USING CHIRP FUNCTION
#define CHIRP_4KHZ 125  ///TO GENERATE 4KHZ TONE USING CHIRP FUNCTION
