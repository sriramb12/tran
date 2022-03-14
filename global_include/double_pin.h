#ifndef mask_pin_h
#define mask_pin_h

#include "global_defines.h"

typedef struct
{
    char pin1[MAX_PIN_LEN+1];
    char pin2[MAX_PIN_LEN+1];
    char override_flag;
    float vnum;
    float sec_vnum;
    char version_flag;
    char sec_version_flag;
} DOUBLE_PIN;

extern DOUBLE_PIN tmp_pin;

#ifdef ANSI_C
void
mask_pin_set(
    char *pin1,
    char *pin2);

void
mask_pin_init(void);

#endif /* define for ANSI_C */
#endif /* Define for double_pin_h */
