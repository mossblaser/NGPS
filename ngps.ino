/**
 * A proof of concept for the "Non-Global Navigation System". Uses a single
 * arduino and three unmodified HC-SR04 modules.
 * The principle of the calculation:
 *
 *                   A_B_SEP_CM (AB)
 *                     A ------- B
 *                       \a   b/
 *    uS_B/CM_PER_US (AC) \   / uS_B/CM_PER_US (BC)
 *                         \ /
 *                          C
 *
 * Using the cosine rule:
 *
 *   Angle a = acos((-(BC*BC) + (AB*AB) + (AC*AC)) / (2*AB*AC))
 */

#include <math.h>
#include <NewPing.h>

// The pins used by the modules
#define A_TRIGGER_PIN  12
#define A_ECHO_PIN     11
#define B_TRIGGER_PIN  A0
#define B_ECHO_PIN     A1
#define C_TRIGGER_PIN  A2
#define C_ECHO_PIN     A3

// Allows the ping listener to timeout after an appropriate delay
#define MAX_DISTANCE 500

// (Fixed) distance between module A and B (cm)
#define A_B_SEP_CM 70.0

// Number of microseconds required travel one CM at the speed of sound
#define CM_PER_US 29.3866996

NewPing sonar(C_TRIGGER_PIN, C_ECHO_PIN, MAX_DISTANCE);

void setup() {
	Serial.begin(115200);
	pinMode(A_TRIGGER_PIN, OUTPUT);
	pinMode(B_TRIGGER_PIN, OUTPUT);
	pinMode(C_TRIGGER_PIN, OUTPUT);
	pinMode(A_ECHO_PIN, INPUT);
	pinMode(B_ECHO_PIN, INPUT);
	pinMode(C_ECHO_PIN, INPUT);
}

void loop() {
	// Cause A to ping and measure how long it takes to get to C
	delay(50);
	digitalWrite(A_TRIGGER_PIN, HIGH);
	digitalWrite(A_TRIGGER_PIN, LOW);
	int uS_A = sonar.ping();

	// Cause B to ping and measure how long it takes to get to C
	delay(5);
	digitalWrite(B_TRIGGER_PIN, HIGH);
	digitalWrite(B_TRIGGER_PIN, LOW);
	int uS_B = sonar.ping();

	// Cause C to ping just to sanity check that the module is working in some
	// fashion. (XXX: Not really needed...)
	delay(5);
	int uS_C = sonar.ping();
	
	// Use cosine rule, yo'
	double ab = A_B_SEP_CM;
	double ac = uS_A/CM_PER_US;
	double bc = uS_B/CM_PER_US;
	double alpha = acos((-(bc*bc) + (ab*ab) + (ac*ac)) / (2*ab*ac));
	
	// Get some trig in there too, man.
	double x = ac * cos(alpha);
	double y = ac * sin(alpha);
	
	// This simple filter does a surprisingly reasonable job of removing cases
	// where things didn't work.
	if (uS_A != 0 && uS_B != 0 && uS_C != 0 && !isnan(x) && !isnan(y)) {
		Serial.println();
		Serial.print("Ping A: ");
		Serial.print(uS_A / CM_PER_US);
		Serial.print("cm\tPing B: ");
		Serial.print(uS_B / CM_PER_US);
		Serial.print("cm\tPing C: ");
		Serial.print(uS_C / CM_PER_US);
		Serial.print("cm\tX: ");
		Serial.print(x);
		Serial.print("cm\tY: ");
		Serial.print(y);
		Serial.print("cm");
	} else {
		Serial.print(".");
	}
}
