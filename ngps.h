#ifndef NGPS_H
#define NGPS_H

/**
 * Serial debugging speed.
 */
#define SERIAL_BAUD_RATE 115200


/******************************************************************************
 * Pin definitions
 ******************************************************************************/

/**
 * Pin used to synchronise the clock of a receiver from the satellite. Active
 * low.
 */
#define SYNC_PIN 2
#define SYNC_INTERRUPT 0


/**
 * The pin on the receiver connected to the HC-SR04's comparator output (rather
 * than the usual "echo" output).
 */
#define REC_ECHO_PIN 3
#define REC_ECHO_INTERRUPT 1


/**
 * The pin on the receiver connected to the HC-SR04's trigger.
 */
#define REC_TRIGGER_PIN 4


/**
 * A list of pins on a satellite connected to HC-SR04 board trigger pins. The
 * board's echo pins are simply ignored.
 */
#define SAT_TRIGGER_PINS ((int[]){ A0 \
                                 , A1 \
                                 })

/**
 * The number of transmitters in the system (just the number of transmitter
 * pins).
 */
#define NUM_TRANSMITTERS (sizeof(SAT_TRIGGER_PINS)/sizeof(SAT_TRIGGER_PINS[0]))



/******************************************************************************
 * Physical constants.
 ******************************************************************************/

/**
 * Number of microseconds required travel one centimeter at the speed of sound.
 */
#define US_PER_CM 29.3866996


/**
 * Maximum distance in centimeters a ping can travel (possibly including
 * reflection) and still be picked up by the HC-SR04.
 */
 #define MAX_PING_DISTANCE 1000.0


/******************************************************************************
 * Ping State-machine Parameters
 ******************************************************************************/

typedef int fsm_state_t;

/**
 * FSM states identifiers.
 */
#define FSM_STATE_SYNC ((fsm_state_t) 0)

#define TRANSMITTER_TO_FSM_STATE(sat_num) ((fsm_state_t) (1 + (sat_num)))
#define FSM_STATE_TO_TRANSMITTER(state)   ((state) - 1)

/**
 * Period of FSM clock (us). This should be long enough that any ping
 * reflections should have made it back by now. Truncated to an integer.
 */
#define FSM_CLOCK_PERIOD ((int)(MAX_PING_DISTANCE * US_PER_CM))


/**
 * The number of FSM states.
 */
#define NUM_FSM_STATES (1 + NUM_TRANSMITTERS)


#endif
