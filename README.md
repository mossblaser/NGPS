Non-Global Navigation System
============================

*A Work-in-progress.* A demonstration of some of the principles behind GPS using
ultrasonic transducers.

Principle of Operation
----------------------

The system consists of a number (at least two) of "satellites" and possibly a
number of mobile receivers.

The satellites consist of (unrealistically) stationary transmitters which
periodically send an ultrasonic "ping". The transponders share a common clock
and transmit their pings at the same interval but with differing (and known)
phases. The phases should be sufficiently different that even if the ping has to
travel a long distance it is not ambiguous which satellite produced it. For
example, if there are three satellites, their pings might look like this:

	Sat 0: ____/\______________________________/\_________________________...
	Sat 1: __________/\______________________________/\___________________...
	Sat 2: ________________/\______________________________/\_____________...

The receiver also shares the common clock (again unrealistically, see
implementation details for reason) and uses this to work out how delayed each of
the pings are. From this it is possible to use trilateration to work out the
position of the receiver.

It is worth explaining that this is not as inaccurate, in some senses, as it
appears. The real GPS satellites essentially repeatedly broadcast a very precise
representation the current time (according to an onboard atomic clock) along
with its current orbital position. A real GPS receiver then uses several
satellite signals to work out the (actual) current time and then compares this
with the time encoded in the GPS signal and this difference combined with
knowledge of the Satellite's position can be used to figure out where the
receiver is.

In this example system, the pings actually encode exactly the same information.
The position of the satellite is encoded in the (approximate) timing of the
ping. The time is also known as the receiver knows the exact phase of each
receiver and thus can determine how late a ping arrived.


Implementation Details
----------------------

The system (mis-)uses commodity hobbyist-friendly HC-SR04 ultrasonic range
finder modules. In normal operation, a "trigger" pin causes the module to emit
an ultrasonic ping which in normal operation will reflect back to the module
causing some on-board logic to pulse the "echo" pin high. The duration of this
pulse indicates the time between the pulse being sent and received by the
module. Unfortunately the module does not listen until you trigger the sending
of a ping.

In this project we instead wish to split the transmit/receive parts of the
process. An easy hack, which doesn't require the modules to be modified, is to
trigger the satellites (and ignore the echo) and for the receiver to also
trigger itself at the same moment as the satellites. Since the ping from the
satellite will (should) arrive at the module before the module's ping is
reflected back the module will indicate the time taken for the satellite's ping
to arrive.

Since an obvious/easy way to trigger both the satellites and receiver at the
same time is for them to share a clock and use that to time their pings. Since
the receiver will be connected to a host to do I/O anyway it makes sense to
simplify implementation by using this connection for clock synchronisation. The
lots-o-effort alternative might be to transmitter from the receiver's module and
then constantly triggering the module to essentially allow constant listening to
pings and then trying to decode the clock from a collection of at least three
receivers.
