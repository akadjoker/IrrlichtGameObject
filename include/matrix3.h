// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_MATRIX3_H_INCLUDED__
#define __IRR_MATRIX3_H_INCLUDED__

#include "irrMath.h"
#include "vector3d.h"
#include "vector2d.h"
#include "plane3d.h"
#include "aabbox3d.h"
#include "rect.h"
#include "irrString.h"






namespace irr
{
namespace core
{

	//! 3x3 matrix. Mostly used as transformation matrix for 3d calculations.
	template <class T>
	class CMatrix3
	{

		public:
		CMatrix3(){
		set(1.0f,0.0f,0.0f,
            0.0f,1.0f,0.0f,
            0.0f,0.0f,1.0f);
		};

	  	CMatrix3(	const T&  	in_d00,const T&  	in_d01,const T&  	in_d02,
                    const T&  	in_d10,const T&  	in_d11,const T&  	in_d12,
                    const T&  	in_d20,const T&  	in_d21,const T&  	in_d22)
			{
				M[0]  = in_d00; M[1]  = in_d01; M[2]  = in_d02;
				M[3]  = in_d10; M[4]  = in_d11; M[5]  = in_d12;
				M[6]  = in_d20; M[7]  = in_d21; M[8]  = in_d22;


			}


	  	void set(	const T&  	in_d00,const T&  	in_d01,const T&  	in_d02,
                    const T&  	in_d10,const T&  	in_d11,const T&  	in_d12,
                    const T&  	in_d20,const T&  	in_d21,const T&  	in_d22)
			{
				M[0]  = in_d00; M[1]  = in_d01; M[2]  = in_d02;
				M[3]  = in_d10; M[4]  = in_d11; M[5]  = in_d12;
				M[6]  = in_d20; M[7]  = in_d21; M[8]  = in_d22;
           }

           		//! Simple operator for directly accessing every element of the matrix.
			T& operator()(const s32 row, const s32 col)
			{
				return M[ row * 4 + col ];
			}

			//! Simple operator for directly accessing every element of the matrix.
			const T& operator()(const s32 row, const s32 col) const { return M[row * 3 + col]; }

			//! Simple operator for linearly accessing every element of the matrix.
			T& operator[](u32 index)
			{
				return M[index];
			}

			//! Simple operator for linearly accessing every element of the matrix.
			const T& operator[](u32 index) const { return M[index]; }

						//! Returns pointer to internal array
			const T* pointer() const { return M; }
			T* pointer()
			{
				return M;
			}

            /// Test for equality with another matrix without epsilon.
        bool operator ==(const CMatrix3<T>& rhs) const
        {
            const float* leftData = pointer();
            const float* rightData = rhs.pointer();

            for (unsigned i = 0; i < 9; ++i)
            {
                if (leftData[i] != rightData[i])
                    return false;
            }

            return true;
        }

    /// Test for inequality with another matrix without epsilon.
     bool operator !=(const CMatrix3<T>& rhs) const { return !(*this == rhs); }

			private:
			//! Matrix data, stored in row-major order
			T M[9];


    };






    	//! Typedef for f32 matrix
	typedef CMatrix3<f32> matrix3;

	//! global const identity matrix
//	IRRLICHT_API extern const matrix3 IdentityMatrix3;

} // end namespace core
} // end namespace irr

#endif


				/*

in_d00	Value to set in this matrix[0,0].
in_d01	Value to set in this matrix[0,1].
in_d02	Value to set in this matrix[0,2].
in_d10	Value to set in this matrix[1,0].
in_d11	Value to set in this matrix[1,1].
in_d12	Value to set in this matrix[1,2].
in_d20	Value to set in this matrix[2,0].
in_d21	Value to set in this matrix[2,1].
in_d22	Value to set in this matrix[2,2].*/
