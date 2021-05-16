#include <Arduino.h>
#include <esp32-hal-ledc.h>

#define MOT_A1_PIN 18
#define MOT_A2_PIN 19
#define MOT_B1_PIN 23
#define MOT_B2_PIN 22
#define MOT_C_PIN 13

void attachMotorPin(uint8_t ch, uint8_t pin) 
{ 
    ledcSetup(ch, 5000, 8);
    ledcAttachPin(pin, ch); 
}

void attachServoPin(uint8_t ch, uint8_t pin) 
{ 
    ledcSetup(ch, 50, 16);
    ledcAttachPin(pin, ch); 
}

void servoWrite(int us, uint8_t ch)
{
  ledcWrite(ch, map(us, 0, 20000, 0, 65535));
}

void motorWrite(int pwm, uint8_t ch1, uint8_t ch2)
{
  if (pwm < 0) {  // reverse speeds
    ledcWrite(ch1, -pwm);
    ledcWrite(ch2, 0);

  } else if (pwm > 0) { // stop or forward
    ledcWrite(ch1, 0);
    ledcWrite(ch2, pwm);
  }
  else {    
    ledcWrite(ch1, 0);
    ledcWrite(ch2, 0);
  }
}

void rcc_init()
{
    attachMotorPin(0, MOT_A1_PIN);
    attachMotorPin(1, MOT_A2_PIN);
    attachMotorPin(2, MOT_B1_PIN);
    attachMotorPin(3, MOT_B2_PIN);
    attachServoPin(4, MOT_C_PIN);
}

void rcc_set_speed(int left, int right)
{
    int a = constrain(left, -255, 255);
    int b = constrain(right,-255, 255);
    motorWrite(a, 0, 1);
    motorWrite(b, 2, 3);
}

void rcc_set_steering(int pos)
{
    int x = constrain(pos, -90, 90);
    servoWrite(map(x, -90, 90, 600, 2400), 4);
}