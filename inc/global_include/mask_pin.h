#ifndef mask_pin_h
#define mask_pin_h

#include "global_defines.h"

typedef struct
{
    char pin1[MAX_PIN_LEN+1];
    char pin2[MAX_PIN_LEN+1];
} MASK_PIN;

extern MASK_PIN pin;

void
mask_pin_set(
    char *pin1,
    char *pin2);

void
mask_pin_init(void);

#endif /* Define for mask_pin_h */
