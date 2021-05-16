#pragma once
#include <Arduino.h>

void rcc_init();
void rcc_set_speed(int left, int right);
void rcc_set_steering(int pos);