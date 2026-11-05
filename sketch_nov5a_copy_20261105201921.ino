#include <AccelStepper.h>

// ============================================
// HALL SENSOR + STEPPER MOTOR RPM CALIBRATION
// ESP32 + ULN2003 + 28BYJ-48
// ============================================

// ULN2003 pins
#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19

// Hall sensor
#define HALL_PIN 34

// Motor configuration
// Change this to test different speeds
const float MOTOR_TEST_SPEED = 800.0;

// Hall sensor configuration
// Assumption: 1 magnet = 1 pulse per revolution
const float PULSES_PER_REV = 1.0;

// AccelStepper pin order for 28BYJ-48
AccelStepper motor(
  AccelStepper::HALF4WIRE,
  IN1, IN3, IN2, IN4
);

// Hall pulse counter
volatile unsigned long hallPulses = 0;

// RPM calculation
unsigned long previousTime = 0;
unsigned long previousPulses = 0;

void IRAM_ATTR hallISR() {
  hallPulses++;
}

void setup() {

  Serial.begin(115200);

  // Hall sensor setup
  pinMode(HALL_PIN, INPUT);

  attachInterrupt(
    digitalPinToInterrupt(HALL_PIN),
    hallISR,
    FALLING
  );

  // Motor setup
  motor.setMaxSpeed(1000);
  motor.setSpeed(MOTOR_TEST_SPEED);

  Serial.println();
  Serial.println("================================");
  Serial.println("HALL + MOTOR RPM CALIBRATION");
  Serial.println("================================");

  Serial.print("Motor test speed: ");
  Serial.println(MOTOR_TEST_SPEED);

  Serial.println("Motor starting...");
  Serial.println();

  previousTime = millis();
}

void loop() {

  // Keep motor running continuously
  motor.runSpeed();

  // Calculate RPM every 1 second
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= 1000) {

    unsigned long currentPulses;

    noInterrupts();
    currentPulses = hallPulses;
    interrupts();

    unsigned long pulseDifference =
      currentPulses - previousPulses;

    float elapsedSeconds =
      (currentTime - previousTime) / 1000.0;

    float pulsesPerSecond =
      pulseDifference / elapsedSeconds;

    float rpm =
      (pulsesPerSecond * 60.0) /
      PULSES_PER_REV;

    Serial.println("-------------------------------");

    Serial.print("Total Hall pulses: ");
    Serial.println(currentPulses);

    Serial.print("Pulses per second: ");
    Serial.println(pulsesPerSecond, 2);

    Serial.print("Measured RPM: ");
    Serial.println(rpm, 2);

    Serial.print("Motor commanded speed: ");
    Serial.println(MOTOR_TEST_SPEED);

    previousPulses = currentPulses;
    previousTime = currentTime;
  }
}