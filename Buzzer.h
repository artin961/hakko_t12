class BUZZER {
  public:
    BUZZER() {
      BUZZER_PORT &= ~BUZZER_BITMASK;
      BUZZER_DDR |= BUZZER_BITMASK;
    }
    void activate(bool on) {
      this->_enabled = on;

    }
    void shortBeep(void) {
      if (this->_enabled)
        chirp(50, CHIRP_2KHZ);  // Send 2KHz sound signal...
    }
    void lowBeep(void) {
      if (this->_enabled)
        chirp(150, CHIRP_2KHZ);  // Send 2KHz sound signal...
    }
    void doubleBeep(void) {
      if (this->_enabled) {
        chirp(150, CHIRP_2KHZ);  // Send 2KHz sound signal...
        delay(200);
        chirp(150, CHIRP_2KHZ);  // Send 2KHz sound signal...
      }
    }
    void failedBeep(void) {
      if (this->_enabled) {
        chirp(150, CHIRP_2KHZ);  // Send 2KHz sound signal...
        delay(200);
        chirp(150, CHIRP_2KHZ);  // Send 2KHz sound signal...
        delay(200);
        chirp(150, CHIRP_2KHZ);  // Send 2KHz sound signal...
      }
    }
  private:
    bool _enabled = true;
    void chirp(int playTime, int delayTime) {  ///FUNCTION TO MAKE BUZZES
      long loopTime = (playTime * 1000L) / (delayTime);
      for (int i = 0; i < loopTime; i++) {
        BUZZER_PORT ^= BUZZER_BITMASK;  //TOGGLE BUZZER
        delayMicroseconds(delayTime);
      }
      BUZZER_PORT &= ~BUZZER_BITMASK;  //BUZZER PIN OFF JUST IN CASE
    }
};