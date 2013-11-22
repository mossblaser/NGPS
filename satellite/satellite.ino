#include <Timer1.h>

#include "ngps.h"


/******************************************************************************
 * Ping State-machine Parameters
 ******************************************************************************/

fsm_state_t fsm_state = FSM_STATE_SYNC;


/**
 * Execute one step of the FSM.
 */
void
ping_fsm_tick(void)
{
	if (fsm_state == FSM_STATE_SYNC) {
		// During the sync state just assert the sync line.
		digitalWrite(SYNC_PIN, LOW);
	} else if (   fsm_state >= FSM_STATE_TRANSMIT(0)
	           && fsm_state <  FSM_STATE_TRANSMIT(NUM_TRANSMITTERS)
	          ) {
		// During the transmit states, just pulse the trigger line
		digitalWrite(SAT_TRIGGER_PINS, HIGH);
		digitalWrite(SAT_TRIGGER_PINS, LOW);
		
		// Make sure that the active-low sync signal is deasserted
		digitalWrite(SYNC_PIN, HIGH);
	} else {
		// Unknown state, just reset (and have the sync signal low in
		// preperation)...
		digitalWrite(SYNC_PIN, HIGH);
		fsm_state = FSM_STATE_SYNC;
	}
}



/******************************************************************************
 * Setup/main-loop
 ******************************************************************************/

void
setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	
	// Set up trigger pins
	for (int i = 0; i < NUM_TRANSMITTERS; i++) {
		pinMode(SAT_TRIGGER_PINS[i], OUTPUT);
		digitalWrite(SAT_TRIGGER_PINS[i], LOW);
	}
	
	// Set up the active-low sync pin
	pinMode(SYNC_PIN, OUTPUT);
	digitalWrite(SYNC_PIN, HIGH);
	
	// Setup the FSM
	Timer1.initialize(FSM_CLOCK_PERIOD);
	Timer1.attachInterrupt(ping_fsm_tick);
	Timer1.start();
}

void
loop()
{
	// Nothing to do!
}
