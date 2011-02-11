#ifndef MS5607_H
#define MS5607_H

enum {
  ADDRESS	=	0x77,
};

enum {
  ADC_READ = 0x00,
  CONVERT_TEMPERATURE_FAST = 0x50,
  CONVERT_TEMPERATURE_SLOW = 0x58,
  CONVERT_PRESSURE_FAST = 0x40,
  CONVERT_PRESSURE_SLOW = 0x48,
  PROM_READ_MASK = 0xA0,
} ms5607_command;

enum {
  FAST_WAIT = 1,
  SLOW_WAIT = 10,
} ms5607_mes_delay;
#endif
