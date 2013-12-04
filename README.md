Non-Global Positioning System
=============================

*A Work-in-progress.* A demonstration of some of the principles behind GPS using
ultrasonic transducers.

Principle of Operation
----------------------

The system consists of four "satellites" and possibly a number of mobile
receivers.

The satellites are (unrealistically) stationary transmitters which periodically
send an ultrasonic "ping". The satellites share a common clock and transmit
their pings at the same period but with differing (and known) phases. The phases
should be sufficiently different that even if the ping has to travel a long
distance it is not ambiguous which satellite produced it (e.g. in the presence
of echos). For example, if there are three satellites, their pings might look
like this:

	Sat 0: ____/\___________________________________/\_________________________...
	Sat 1: __________/\___________________________________/\___________________...
	Sat 2: ________________/\___________________________________/\_____________...
	Sat 3: ______________________/\___________________________________/\_______...

The receiver will be able to hear each ping (which it can discern from echos by
only listening to the first ping after a period of silence) but does not know
how long it has taken to get to the receiver. What it does know is how long it
took relative to the other pings it heard. It also knows the positions of the
transmitters and the period and phase of the pings. Using this information alone
it is possible to determine the location (and the time on the transmitter's
clock). A detailed explanation of this process is given in
`./maths/multilateration.wxm`.

With the exception of the stationary satellites, this system is extremely
similar to GPS:

* The GPS satellites each contain an extrememly accurate atomic clock which
  essentially means that all the satellites share a common clock (as is the case
  in this system where instead a single (cheap) clock is physically shared).
* GPS satellites broadcast their current physical location. In this system, the
  order of the pings indicates which satellite sent it (and due to the
  stationary position, this implies the satellite's position).
* GPS signals contain the time as a number of seconds since a certain epoch and
  this system essentially indicates the time as the number of phase-differences
  since the start of the sequence of pings (i.e. also the time since an epoch).
* The system uses (in principle) the same maths to work out the position of a
  receiver given the above information. GPS also has to deal with quantum
  effects due to the signals travelling at the speed of light but since we're
  using (much slower) ultrasound, these effects are not as noticable.


HC-SR04 Ultrasonic Range Finder Hack
------------------------------------

The system (mis-)uses commodity hobbyist-friendly HC-SR04 ultrasonic range
finder modules. In normal operation, a "trigger" pin causes the module to emit a
sequence of ultrasonic pings. The pings will reflect back to the module causing
some onboard microcontroller to pulse the "echo" pin high for a certain duration
indicating how long it took for the emitted sequence of pings to arrive back
back at the module.

Unfortunately the module does not listen for incoming pings until it is
triggered. In fact the situation is even worse as it may reject a sequence of
pings sent by another module if they don't follow exactly the same sequence
(e.g. due to differences in crystals used for timing the sequence).

In this project we instead wish to split the transmit/receive parts of the
process such that satellites only transmit and the receiver only listens for
pings.

Luckily, the modules will produce a ping when the trigger signal is pulsed after
a short (but constant) delay. This means that the tansmit function can be used
independently without modification.

Unfortunately, as described above, receiving is trickier. After some probing
with a multimeter the signal which the onboard microcontroller listens to for
incoming pings was found. By soldering an extra wire to this pin it was possible
to listen directly to incoming pings as they arrived, bypassing the
microcontroller. For some reason, this signal is disabled until the module has
sent at least one ping. The reason for this behaviour is unknown but one
hypothesis is that the onboard software powers-off the receiever until at least
one ping has been sent. The motivation for doing this, however, is unclear since
it never appears to disable the receiver again and thus it doesn't seem to be a
power-saving feature.


Files
-----

### `./`

* `./README.md` See the README for more info.
* `./ngps.h` The shared headerfile for the transmitter and receiver Arduino
  sketches.

### `./receiver/` and `./satellite/`

Arduino sketches implementing the receiver and satellites.

Note: A symlink called `ngps.h` linking to `./ngps.h` should be included in
these directories to allow the Arduino compiler to find it (grrr).

### `./maths/`

Workings and playing around with the maths behind the fun problems required for
calculating a fix.

* `./maths/trilateration.wxm` A brute-force (Wx)Maxima workthrough of a solution
  to the general trilateration problem.

* `./maths/multilateration.wxm` A (Wx)Maxima workthrough of a solution to the
  constrained (but general) multilateration problem.

* `./maths/multilateration.py` A demonstrator of the increasingly well defined
  solutions gained using multilateration.

