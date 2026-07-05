#ifndef __INIT_H__
#define __INIT_H__

#include "global_config.h"

#include "timer.h"
#include "uart.h"
#include "atim.h"
#include "wdt.h"
#include "delay.h"
#include "i2c.h"

void mcu_init(void);

static void init_system(void);
static void init_variables(void);
static void init_gpios(void);
static void init_peripherals(void);

#endif
