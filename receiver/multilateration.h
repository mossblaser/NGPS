#ifndef MULTILATERATION_H
#define MULTILATERATION_H

/**
 * Given the actual coordinates of three points, ac_{x,y,z}, bc_{x,y,z},
 * cc_{x,y,z}, calculate two transformation matrices.
 *
 * to_normal_form will be set to a transformation matrix which will cause
 * ac_{x,y,z} to lie on the origin, bc_{x,y,z} to lie on the x-axis and
 * cc_{x,y,z} to lie on the x-y-plane.
 *
 * from_normal_form will transform back to the original coordinates.
 *
 * to_normal_form and from_normal_form are matrices for use with the Arduino
 * MatrixMath library. They should each be an array of 4*4 floats.
 */
void
calculate_normalisation_matrices( float *to_normal_form
                                , float *from_normal_form
                                , float ac_x, float ac_y, float ac_z
                                , float bc_x, float bc_y, float bc_z
                                , float cc_x, float cc_y, float cc_z
                                );


/**
 * Given a system where we know the relative distances between us and four
 * points (ar, br, cr, dr) and the location of those four points, determine our
 * position (x,y,z) and the amount of error between our relative distances and
 * the actual distances (e).
 *
 * To simplify the calculations, the four points must be constrained as follows:
 * * Point a must be at the origin (0,0,0)
 * * Point b must lie on the x-axis (bc_x,0,0) and not on the origin.
 * * Point c must lie on the x-y-axis plane (cc_x,cc_y,0) and not on the x-axis.
 * * Point d must may lie anywhere in 3D space (dc_x,dc_y,dc_z) excluding the
 *   x-y plane.
 *
 * See calculate_normalisation_matrices(...) for a transformation from the
 * general case to the above constrained case (and back).
 *
 * Since the system being solved is quadratic, there exist two possible solution
 * positions, "1" and "2". The argument solution_number selects which solution
 * to yeild.
 */
void
multilateration( float ar
               , float br, float bc_x
               , float cr, float cc_x, float cc_y
               , float dr, float dc_x, float dc_y, float dc_z
               , float *e, float *x, float *y, float *z
               , int solution_number
               );

#endif
