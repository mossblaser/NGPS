#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "MatrixMath.h"

#include "multilateration.h"

/******************************************************************************
 * Internal Functions/Definitions
 ******************************************************************************/

#ifndef PI
#define PI 3.14159265359
#endif


/**
 * Defines a transformation matrix for a translation.
 */
#define T_MAT_TRANSLATION(d) { 1.0f, 0.0f, 0.0f, (d)[0] \
                             , 0.0f, 1.0f, 0.0f, (d)[1] \
                             , 0.0f, 0.0f, 1.0f, (d)[2] \
                             , 0.0f, 0.0f, 0.0f,   1.0f \
                             }

/**
 * Defines transformation matrices for rotations around the three axes.
 */
#define T_MAT_ROTATE_X(a) {    1.0f,    0.0f,    0.0f,    0.0f \
                          ,    0.0f, cosf(a), sinf(a),    0.0f \
                          ,    0.0f,-sinf(a), cosf(a),    0.0f \
                          ,    0.0f,    0.0f,    0.0f,    1.0f \
                          }

#define T_MAT_ROTATE_Y(a) { cosf(a),    0.0f,-sinf(a),    0.0f \
                          ,    0.0f,    1.0f,    0.0f,    0.0f \
                          , sinf(a),    0.0f, cosf(a),    0.0f \
                          ,    0.0f,    0.0f,    0.0f,    1.0f \
                          }

#define T_MAT_ROTATE_Z(a) { cosf(a), sinf(a),    0.0f,    0.0f \
                          ,-sinf(a), cosf(a),    0.0f,    0.0f \
                          ,    0.0f,    0.0f,    1.0f,    0.0f \
                          ,    0.0f,    0.0f,    0.0f,    1.0f \
                          }


/**
 * Return either 1 or -1 depending on whether the input number is positive or
 * negative respectively. If zero, result should not be relied on.
 */
static float sign(float num) {
	return (num < 0.0f) ? -1.0f : 1.0f;
}




/******************************************************************************
 * Public Functions
 ******************************************************************************/

void
calculate_normalisation_matrices( float *to_normal_form
                                , float *from_normal_form
                                , float ac_x, float ac_y, float ac_z
                                , float bc_x, float bc_y, float bc_z
                                , float cc_x, float cc_y, float cc_z
                                )
{
	//--------------------------------------
	// Calculate the "to_normal_form" matrix
	//--------------------------------------
	
	// Translate such that the first point lies on the origin
	float trans_a_to_o[4*4] = T_MAT_TRANSLATION(((float[3]){-ac_x, -ac_y, -ac_z}));
	
	// Rotate to get b on the x-axis in two parts. First rotate onto the x-y
	// axis...
	float b[4] = {bc_x, bc_y, bc_z, 1.0f};
	float b_trans[4];
	Matrix.Multiply(trans_a_to_o, b, 4, 4, 1, b_trans);
	float angle_b_trans_to_xy_about_x = b_trans[1]!=0.0f ? atanf(b_trans[2] / b_trans[1])
	                                                     : sign(b_trans[2]*PI/2.0f)
	                                                     ;
	float rot_b_trans_to_xy[4*4] = T_MAT_ROTATE_X(angle_b_trans_to_xy_about_x);
	// ...second, rotate onto the x axis
	float b_trans_rot[4];
	Matrix.Multiply(rot_b_trans_to_xy, b_trans, 4, 4, 1, b_trans_rot);
	float angle_b_trans_rot_to_x_about_z = b_trans_rot[0]!=0.0f ? atanf(b_trans_rot[1] / b_trans_rot[0])
	                                                            : sign(b_trans_rot[1]*PI/2.0f)
	                                                            ;
	float rot_b_trans_rot_to_x[4*4] = T_MAT_ROTATE_Z(angle_b_trans_rot_to_x_about_z);
	// Combine the two rotations into one
	float rot_b_trans_to_x[4*4];
	Matrix.Multiply(rot_b_trans_rot_to_x, rot_b_trans_to_xy, 4, 4, 4, rot_b_trans_to_x);
	// Combine the first translation and the rotation into one
	float trans_rot_a_to_o_b_to_x[4*4];
	Matrix.Multiply(rot_b_trans_to_x, trans_a_to_o, 4, 4, 4, trans_rot_a_to_o_b_to_x);
	
	// Rotate to get cc onto the x-y-plane
	float c[4] = {cc_x, cc_y, cc_z, 1.0f};
	float c_trans_rot[4];
	Matrix.Multiply(trans_rot_a_to_o_b_to_x, c, 4, 4, 1, c_trans_rot);
	float angle_c_trans_rot_to_xy_about_x = c_trans_rot[1]!=0.0f ? atanf(c_trans_rot[2]/c_trans_rot[1])
	                                                             : sign(c_trans_rot[2]*PI/2.0f)
	                                                             ;
	float rot_c_trans_rot_to_xy[4*4] = T_MAT_ROTATE_X(angle_c_trans_rot_to_xy_about_x);
	// Combine this transformation with the first two to yeild the final
	// transformation matrix
	Matrix.Multiply(rot_c_trans_rot_to_xy, trans_rot_a_to_o_b_to_x, 4, 4, 4, to_normal_form);
	
	//-----------------------------------------
	// Calculate the matrix to revert the above
	//-----------------------------------------
	
	// Rotate c back off the x-y-plane
	float rot_c_trans_rot_from_xy[4*4] = T_MAT_ROTATE_X(-angle_c_trans_rot_to_xy_about_x);
	
	// Rotate b back onto the x-y-plane (from the x-axis)
	float rot_b_trans_rot_from_x[4*4] = T_MAT_ROTATE_Z(-angle_b_trans_rot_to_x_about_z);
	// Combine with the first transformation
	float rot_c_trans_rot_from_xy_b_trans_rot_from_x[4*4];
	Matrix.Multiply( rot_b_trans_rot_from_x, rot_c_trans_rot_from_xy
	               , 4, 4, 4
	               , rot_c_trans_rot_from_xy_b_trans_rot_from_x
	               );
	
	// Rotate and then rotate b back off the x-y-plane entirely
	float rot_b_trans_rot_from_xy[4*4] = T_MAT_ROTATE_X(-angle_b_trans_to_xy_about_x);
	// Combine with the above
	float rot_c_trans_rot_from_xy_b_trans_rot_from_xy[4*4];
	Matrix.Multiply( rot_b_trans_rot_from_xy, rot_c_trans_rot_from_xy_b_trans_rot_from_x
	               , 4, 4, 4
	               , rot_c_trans_rot_from_xy_b_trans_rot_from_xy
	               );
	
	// Translate a back off the origin
	float trans_a_from_o[4*4] = T_MAT_TRANSLATION(((float[3]){ac_x, ac_y, ac_z}));
	// Combine with the above to arrive back where we started
	Matrix.Multiply( trans_a_from_o, rot_c_trans_rot_from_xy_b_trans_rot_from_xy
	               , 4, 4, 4
	               , from_normal_form
	               );
	
}


void
multilateration( float ar
               , float br, float bc_x
               , float cr, float cc_x, float cc_y
               , float dr, float dc_x, float dc_y, float dc_z
               , float *e, float *x, float *y, float *z
               , int solution_number
               )
{
	// The formuae below were derrived as described in the (wx)Maxima file
	// `./maths/multilateration.wxm`. They are copied (nearly) driectly from
	// Maxima's output and as a result are not optimised for readability.
	
	// Coefficients of the quadratics with the indeterminate e defining x, y and z.
	float x_p1 = (br-ar)/bc_x;
	float x_p0 = -(br*br-bc_x*bc_x-ar*ar)/(2.0*bc_x);
	float y_p1 = -(cc_x*x_p1-cr+ar)/cc_y;
	float y_p0 = -(2.0*cc_x*x_p0+cr*cr-cc_y*cc_y-cc_x*cc_x-ar*ar)/(2.0*cc_y);
	float z_p1 = -(dc_y*y_p1+dc_x*x_p1-dr+ar)/dc_z;
	float z_p0 = -(2.0*dc_y*y_p0+2.0*dc_x*x_p0+dr*dr-dc_z*dc_z-dc_y*dc_y-dc_x*dc_x-ar*ar)
	           /(2.0*dc_z);
	
	// The two possible solutions for e.
	if (solution_number == 1) {
		*e = -(sqrt((-y_p0*y_p0-x_p0*x_p0+ar*ar)*z_p1*z_p1
		             +(2.0*y_p0*y_p1+2.0*x_p0*x_p1+2.0*ar)*z_p0*z_p1
		             +(-y_p1*y_p1-x_p1*x_p1+1.0)*z_p0*z_p0+(ar*ar-x_p0*x_p0)*y_p1*y_p1
		             +(2.0*x_p0*x_p1+2.0*ar)*y_p0*y_p1+(1.0-x_p1*x_p1)*y_p0*y_p0
		             +ar*ar*x_p1*x_p1+2.0*ar*x_p0*x_p1+x_p0*x_p0)
		   +z_p0*z_p1+y_p0*y_p1+x_p0*x_p1+ar)
		   /(z_p1*z_p1+y_p1*y_p1+x_p1*x_p1-1.0);
	} else if (solution_number == 2) {
		*e = (sqrt((-y_p0*y_p0-x_p0*x_p0+ar*ar)*z_p1*z_p1
		            +(2.0*y_p0*y_p1+2.0*x_p0*x_p1+2.0*ar)*z_p0*z_p1
		            +(-y_p1*y_p1-x_p1*x_p1+1.0)*z_p0*z_p0+(ar*ar-x_p0*x_p0)*y_p1*y_p1
		            +(2.0*x_p0*x_p1+2.0*ar)*y_p0*y_p1+(1.0-x_p1*x_p1)*y_p0*y_p0+ar*ar*x_p1*x_p1
		            +2.0*ar*x_p0*x_p1+x_p0*x_p0)
		   -z_p0*z_p1-y_p0*y_p1-x_p0*x_p1-ar)
		   /(z_p1*z_p1+y_p1*y_p1+x_p1*x_p1-1.0);
	} else {
		assert(0);
	}
	
	// Yield the position based on the above choice of e.
	*x = *e*x_p1+x_p0;
	*y = *e*y_p1+y_p0;
	*z = *e*z_p1+z_p0;
}
