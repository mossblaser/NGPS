#include <TimerOne.h>

#include "ngps.h"


/******************************************************************************
 * Ping State-machine Parameters
 ******************************************************************************/

volatile fsm_state_t fsm_state;

/**
 * A constant which specifies that an arriving ping came too late.
 */
#define INVALID_PING_DELAY (-1ul)

/**
 * The offset from the FSM clock at which echos were heard in microseconds. If
 * no ping arrived, the field will be set to INVALID_PING_DELAY.
 */
volatile unsigned long ping_delays[NUM_TRANSMITTERS];


/**
 * Sync falling-edge interrupt.
 */
void
on_sync(void)
{
	// Just reset the FSM
	fsm_state = FSM_STATE_SYNC - 1;
	TCNT1 = 0;
}


/**
 * Echo rising-edge interrupt.
 */
void
on_echo(void)
{
	// Don't run if not a receive state or if a ping already arrived.
	if (fsm_state == FSM_STATE_SYNC
	    || ping_delays[FSM_STATE_TO_TRANSMITTER(fsm_state)] != INVALID_PING_DELAY)
		return;
	
	// Record the time the ping arrived.
	ping_delays[FSM_STATE_TO_TRANSMITTER(fsm_state)] = Timer1.read();
}


/**
 * Execute one step of the FSM.
 */
void
ping_fsm_tick()
{
	digitalWrite(13, !digitalRead(13));
	
	// Advance to the next state
	if (++fsm_state >= NUM_FSM_STATES)
		fsm_state = FSM_STATE_SYNC;
	
	if (fsm_state == FSM_STATE_SYNC) {
		// Trigger the receiver's HC-SR04 module. The echo to this will be ignored
		// but unfortunately the comparator on the board doesn't start listening
		// until at least one ping has been sent. By sending a ping during the sync
		// (idle) cycle we guaruntee that the module is ready to listen in the later
		// cycles, no matter if it is reset.
		digitalWrite(REC_TRIGGER_PIN, HIGH);
		digitalWrite(REC_TRIGGER_PIN, LOW);
	} else if (   fsm_state >= TRANSMITTER_TO_FSM_STATE(0)
	           && fsm_state <  TRANSMITTER_TO_FSM_STATE(NUM_TRANSMITTERS)
	          ) {
		ping_delays[FSM_STATE_TO_TRANSMITTER(fsm_state)] = INVALID_PING_DELAY;
		// During the transmit states just get ready to receive an echo.
		attachInterrupt(REC_ECHO_INTERRUPT, on_echo, RISING);
	} else {
		// Unknown state, just enter the sync state and do nothing.
		fsm_state = FSM_STATE_SYNC;
	}
}



/******************************************************************************
 * Setup/main-loop
 ******************************************************************************/

void
setup()
{
	Timer1.initialize();
	Serial.begin(SERIAL_BAUD_RATE);
	
	pinMode(13, OUTPUT);
	
	// Set up trigger pin
	pinMode(REC_TRIGGER_PIN, OUTPUT);
	digitalWrite(REC_TRIGGER_PIN, LOW);
	
	// Set up echo pin (interrupt is added on demand by the FSM)
	pinMode(REC_ECHO_PIN, INPUT);
	
	// Set up the active-low, pulled-up sync pin
	pinMode(SYNC_PIN, INPUT);
	digitalWrite(SYNC_PIN, HIGH);
	attachInterrupt(SYNC_INTERRUPT, on_sync, FALLING);
	
	// Setup the FSM
	fsm_state = FSM_STATE_SYNC - 1;
	Timer1.attachInterrupt(ping_fsm_tick, FSM_CLOCK_PERIOD);
}

void
loop()
{
	// Wait until the sync state is entered...
	while (fsm_state != FSM_STATE_SYNC)
		;
	
	// Then print the delays
	for (int i = 0; i < NUM_TRANSMITTERS; i++) {
		Serial.print(i);
		Serial.print(": ");
		int delay = ping_delays[i];
		Serial.print(delay);
		Serial.print("\t");
	}
	Serial.println();
	
	// Rate limit myself...
	delay(500);
}

