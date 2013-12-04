#!/usr/bin/env python

"""
A program which (partially) solves a multilateration problem and for the
unsolved parts just scans a range of possible values. By Plotting the output
with different variables of the problem solved the manner in which the
solution-space is (eventually) narrowed down to two points is discerned.

Usage:

	python multilateration.py solve_e solve_x solve_y solve_z

e.g.

	python multilateration.py True False False True

Where `solve_{e,x,y,z}` is a boolean (i.e. True, False) stating whether a given
term should be solved.

This may then be plotted using gnuplot for example with:

	set title "Possible Position Solutions";
	set xlabel "X";
	set ylabel "Y";
	set zlabel "Z";
	set cblabel "Timing Error";
	splot "points.dat" using 1:2:3:4 palette notitle;

This plots the various points colour-coded by the timing error assumed.
"""

import sys

from math import sqrt

def multilateration( ar
                   , br, bc_x
                   , cr, cc_x, cc_y
                   , dr, dc_x, dc_y, dc_z
                   , e = None, x = None, y = None, z = None
                   , e_solution_num = 1
                   ):
	"""
	Perform multilateration to find the timing error, e, and the current
	coordinates x, y, and z. If values are given for e, x, y or z, use them,
	otherwise work these out.
	"""
	# These formuae were worked out in Maxima
	x_p1 = (br-ar)/bc_x
	x_p0 = -(br*br-bc_x*bc_x-ar*ar)/(2.0*bc_x)
	y_p1 = -(cc_x*x_p1-cr+ar)/cc_y
	y_p0 = -(2.0*cc_x*x_p0+cr*cr-cc_y*cc_y-cc_x*cc_x-ar*ar)/(2.0*cc_y)
	z_p1 = -(dc_y*y_p1+dc_x*x_p1-dr+ar)/dc_z
	z_p0 = -(2.0*dc_y*y_p0+2.0*dc_x*x_p0+dr*dr-dc_z*dc_z-dc_y*dc_y-dc_x*dc_x-ar*ar) \
	     /(2.0*dc_z)
	
	if e is None:
		if e_solution_num == 1:
			e = -(sqrt((-y_p0*y_p0-x_p0*x_p0+ar*ar)*z_p1*z_p1 \
			            +(2.0*y_p0*y_p1+2.0*x_p0*x_p1+2.0*ar)*z_p0*z_p1 \
			            +(-y_p1*y_p1-x_p1*x_p1+1.0)*z_p0*z_p0+(ar*ar-x_p0*x_p0)*y_p1*y_p1 \
			            +(2.0*x_p0*x_p1+2.0*ar)*y_p0*y_p1+(1.0-x_p1*x_p1)*y_p0*y_p0 \
			            +ar*ar*x_p1*x_p1+2.0*ar*x_p0*x_p1+x_p0*x_p0) \
			  +z_p0*z_p1+y_p0*y_p1+x_p0*x_p1+ar) \
			  /(z_p1*z_p1+y_p1*y_p1+x_p1*x_p1-1.0)
		elif e_solution_num == 2:
			e = (sqrt((-y_p0*y_p0-x_p0*x_p0+ar*ar)*z_p1*z_p1 \
			           +(2.0*y_p0*y_p1+2.0*x_p0*x_p1+2.0*ar)*z_p0*z_p1 \
			           +(-y_p1*y_p1-x_p1*x_p1+1.0)*z_p0*z_p0+(ar*ar-x_p0*x_p0)*y_p1*y_p1 \
			           +(2.0*x_p0*x_p1+2.0*ar)*y_p0*y_p1+(1.0-x_p1*x_p1)*y_p0*y_p0+ar*ar*x_p1*x_p1 \
			           +2.0*ar*x_p0*x_p1+x_p0*x_p0) \
			  -z_p0*z_p1-y_p0*y_p1-x_p0*x_p1-ar) \
			  /(z_p1*z_p1+y_p1*y_p1+x_p1*x_p1-1.0)
	if x is None:
		x = e*x_p1+x_p0
	if y is None:
		y = e*y_p1+y_p0
	if z is None:
		z = e*z_p1+z_p0
	
	return (e, x, y, z)


def distance(x1,y1,z1 , x2,y2,z2):
	"""
	Find the distance between two 3D points.
	"""
	return sqrt( (x1-x2)*(x1-x2)
	           + (y1-y2)*(y1-y2)
	           + (z1-z2)*(z1-z2)
	           )


# The positions of three transmitters
bc_x = 100.0;
cc_x =   0.0; cc_y = 100.0;
dc_x =   0.0; dc_y = -20.0; dc_z = 20.0;

# The true values of the receiver's position and timing error.
e_actual = 3.0;
x_actual = 400.0;
y_actual = 500.0;
z_actual = 600.0;

# Calculate timings as they might be read read by a receiver.
ar = distance( 0.0,  0.0, 0.0, x_actual, y_actual, z_actual) + e_actual;
br = distance(bc_x,  0.0, 0.0, x_actual, y_actual, z_actual) + e_actual;
cr = distance(cc_x, cc_y, 0.0, x_actual, y_actual, z_actual) + e_actual;
dr = distance(dc_x, dc_y,dc_z, x_actual, y_actual, z_actual) + e_actual;

# Which values should be solved? (Those which aren't solved will have a range of
# values tried and plotted).
e_fix = sys.argv[1] == "True"
z_fix = sys.argv[2] == "True"
y_fix = sys.argv[3] == "True"
x_fix = sys.argv[4] == "True"

# Enumerate possible values of x, y and z.
for e in range(-1000, 1000, 100):
	for y in range(-1000, 1000, 100):
		for x in range(-1000, 1000, 100):
			for z in range(-1000, 1000, 100):
				try:
					e_,x_,y_,z_ = multilateration( ar
					                             , br, bc_x
					                             , cr, cc_x, cc_y
					                             , dr, dc_x, dc_y, dc_z
					                             , None if e_fix else e
					                             , None if x_fix else x
					                             , None if y_fix else y
					                             , None if z_fix else z
					                             , 1
					                             )
					print "%f\t%f\t%f\t%f"%(x_, y_, z_, e_)
					e_,x_,y_,z_ = multilateration( ar
					                             , br, bc_x
					                             , cr, cc_x, cc_y
					                             , dr, dc_x, dc_y, dc_z
					                             , None if e_fix else e
					                             , None if x_fix else x
					                             , None if y_fix else y
					                             , None if z_fix else z
					                             , 2
					                             )
					print "%f\t%f\t%f\t%f"%(x_, y_, z_, e_)
				except ValueError:
					pass # No valid values here
				if z_fix:
					break
			if x_fix:
				break
		if y_fix:
			break
	if e_fix:
		break
