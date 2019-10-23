#ifndef IOT_DEBUG_FUNCTIONS
#define IOT_DEBUG_FUNCTIONS

#include "Arduino.h"

class Debug {
  public:
    static void print(const String& text) {
#ifdef IOT_DEBUG
        Serial.print(text);
#endif
    }

    static void print(const long num) {
#ifdef IOT_DEBUG
        Serial.print(num);
#endif
    }

    static void println(const String& text) {
#ifdef IOT_DEBUG
        Serial.println(text);
#endif
    }

    static void println(const long num) {
#ifdef IOT_DEBUG
        Serial.println(num);
#endif
    }
};

#endif