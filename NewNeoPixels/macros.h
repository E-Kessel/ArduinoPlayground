/*******************************************************

Good Macros:
  EVER      - Use in for loops to loop forever
  LENGTH(x) - Returns the number of items in an array
  LAST(x)   - Returns index of last item in array

********************************************************/

#if ! defined(MACROS_H)
#define MACROS_H

#define EVER (;;)
#define LENGTH(x) (sizeof(x)/sizeof(x[0]))
#define LAST(x) (LENGHT(x)-1)

#endif
