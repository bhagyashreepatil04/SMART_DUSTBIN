#include <Servo.h>

Servo lidServo;

// ---------------- PIN SETUP ----------------
// Hand detection sensor (mounted facing outward, for touchless gesture)
#define HAND_TRIG 2
#define HAND_ECHO 3

// Garbage level sensor (mounted facing down, inside bin)
#define LEVEL_TRIG 4
#define LEVEL_ECHO 5

// Servo (lid)
#define SERVO_PIN 6

// LED and buzzer (fill alert)
#define LED_PIN 7
#define BUZZER_PIN 8

// ---------------- CALIBRATION ----------------
// Distance (cm) from the LEVEL sensor to the BOTTOM of the bin when EMPTY.
// Measure this once with nothing inside the bin and set it here.
// Measured empty reading for this bin: 23 cm.
#define BIN_HEIGHT_CM 23.0

// Percentage full at which LED + buzzer should turn on
#define FULL_THRESHOLD_PERCENT 80.0

// Hand gesture trigger distance (cm)
#define HAND_TRIGGER_CM 20

// How long the lid stays open after a valid gesture (ms)
#define LID_OPEN_TIME 3000

// Servo angles for lid position. If your lid moves backwards
// (opens on hand-away, closes on hand-near), swap these two values.
#define LID_OPEN_ANGLE 0
#define LID_CLOSED_ANGLE 90

// ---------------- STATE ----------------
bool lidOpen = false;
unsigned long lidOpenedAt = 0;

int getDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout ~ 5m range

  if (duration == 0)
    return 999; // no echo = treat as "far / empty"

  return duration * 0.034 / 2;
}

// Takes several samples and returns the median, to reject single-reading noise
int getStableDistance(int trigPin, int echoPin, int samples = 5)
{
  int readings[samples];
  for (int i = 0; i < samples; i++)
  {
    readings[i] = getDistance(trigPin, echoPin);
    delay(15);
  }

  // simple insertion sort (samples is small)
  for (int i = 1; i < samples; i++)
  {
    int key = readings[i];
    int j = i - 1;
    while (j >= 0 && readings[j] > key)
    {
      readings[j + 1] = readings[j];
      j--;
    }
    readings[j + 1] = key;
  }

  return readings[samples / 2]; // median
}

void setup()
{
  Serial.begin(9600);

  pinMode(HAND_TRIG, OUTPUT);
  pinMode(HAND_ECHO, INPUT);

  pinMode(LEVEL_TRIG, OUTPUT);
  pinMode(LEVEL_ECHO, INPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lidServo.attach(SERVO_PIN);
  lidServo.write(LID_CLOSED_ANGLE); // closed
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Smart Dustbin ready.");
}

void loop()
{
  // -------------------------
  // HAND GESTURE -> LID
  // -------------------------
  if (!lidOpen)
  {
    int handDistance = getStableDistance(HAND_TRIG, HAND_ECHO, 3);

    Serial.print("Hand: ");
    Serial.print(handDistance);
    Serial.println(" cm");

    if (handDistance <= HAND_TRIGGER_CM)
    {
      lidServo.write(LID_OPEN_ANGLE);
      lidOpen = true;
      lidOpenedAt = millis();
      Serial.println("Gesture detected -> lid opening");
    }
  }
  else
  {
    // Non-blocking: close the lid after LID_OPEN_TIME has passed,
    // without freezing the rest of the loop (so level sensing still runs)
    if (millis() - lidOpenedAt >= LID_OPEN_TIME)
    {
      lidServo.write(LID_CLOSED_ANGLE);
      lidOpen = false;
      Serial.println("Lid closing");
    }
  }

  // -------------------------
  // FILL LEVEL -> LED + BUZZER
  // -------------------------
  int levelDistance = getStableDistance(LEVEL_TRIG, LEVEL_ECHO, 5);

  // distance shrinks as the bin fills, so convert to a fill percentage
  float fillPercent = (1.0 - (levelDistance / BIN_HEIGHT_CM)) * 100.0;
  fillPercent = constrain(fillPercent, 0, 100);

  Serial.print("Level: ");
  Serial.print(levelDistance);
  Serial.print(" cm  |  Fill: ");
  Serial.print(fillPercent);
  Serial.println(" %");

  if (fillPercent >= FULL_THRESHOLD_PERCENT)
  {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(150);
}