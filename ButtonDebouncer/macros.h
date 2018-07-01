/**********************************************************

Good Macros:
  EVER          - Use in for loops to loop forever
  LENGTH(x)     - Returns the number of items in an array
  LAST(x)       - Returns last item in array
  EVERY(x)      - Use in for loops to loop for every i < x
  DEBUG_PRINT   - Prints only if DEBUG is defined below
  DEBUG_PRINTLN - Print + \n only if DEBUG is defined

***********************************************************/

#if ! defined(MACROS_H)
#define MACROS_H

#define EVER (;;)
#define LENGTH(x) (sizeof(x)/sizeof(x[0]))
#define LAST(x) x[(LENGTH(x)-1)]
#define EVERY(x) (uint32_t i; i < x; i++)

#define DEBUG
#if defined (DEBUG)
  #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
#endif

#endif
