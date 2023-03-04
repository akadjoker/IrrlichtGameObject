#pragma once

struct rmip_t;


#include <string>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include "irrlicht.h"

using namespace irr;
using namespace scene;
using namespace video;


typedef std::string String;

typedef std::vector<String> StringVector;
typedef std::vector< StringVector > StringVectorVector;

typedef std::vector< int > IntVector;
typedef std::vector< char > CharVector;
typedef std::vector< short > ShortVector;
typedef std::vector< unsigned short > UShortVector;
typedef std::queue< int > IntQueue;



template <class Type> class Vector3d
{
//  friend class core::matrix4;
public:
  Vector3d(void) { };  // null constructor, does not inialize point.

  Vector3d(const Vector3d &a) // constructor copies existing vector.
  {
    x = a.x;
    y = a.y;
    z = a.z;
  };

  Vector3d(Type a,Type b,Type c) // construct with initial point.
  {
    x = a;
    y = b;
    z = c;
  };

  bool operator==(const Vector3d<Type> &a) const
  {
    if ( a.x == x && a.y == y && a.z == z ) return true;
    return false;
  };

  bool operator!=(const Vector3d<Type> &a) const
  {
    if ( a.x != x || a.y != y || a.z != z ) return true;
    return false;
  };


// Operators
    Vector3d& operator = (const Vector3d& A)          // ASSIGNMENT (=)
      { x=A.x; y=A.y; z=A.z;
        return(*this);  };

    Vector3d operator + (const Vector3d& A) const     // ADDITION (+)
      { Vector3d Sum(x+A.x, y+A.y, z+A.z);
        return(Sum); };

    Vector3d operator - (const Vector3d& A) const     // SUBTRACTION (-)
      { Vector3d Diff(x-A.x, y-A.y, z-A.z);
        return(Diff); };

    Vector3d operator * (const float s) const       // MULTIPLY BY SCALAR (*)
      { Vector3d Scaled(x*s, y*s, z*s);
        return(Scaled); };

    Vector3d operator / (const float s) const       // DIVIDE BY SCALAR (/)
    {
      float r = 1.0f / s;
        Vector3d Scaled(x*r, y*r, z*r);
        return(Scaled);
    };

    void operator += (const Vector3d A)             // ACCUMULATED VECTOR ADDITION (+=)
      { x+=A.x; y+=A.y; z+=A.z; };
    void operator -= (const Vector3d A)             // ACCUMULATED VECTOR SUBTRACTION (+=)
      { x-=A.x; y-=A.y; z-=A.z; };
    void operator *= (const float s)        // ACCUMULATED SCALAR MULTIPLICATION (*=) (bpc 4/24/2000)
      {x*=s; y*=s; z*=s;}

    Vector3d operator - (void) const                // NEGATION (-)
      { Vector3d Negated(-x, -y, -z);
        return(Negated); };

    Type operator [] (const int i) const         // ALLOWS VECTOR ACCESS AS AN ARRAY.
      { return( (i==0)?x:((i==1)?y:z) ); };
    Type & operator [] (const int i)
      { return( (i==0)?x:((i==1)?y:z) ); };
//

  // accessor methods.
  Type GetX(void) const { return x; };
  Type GetY(void) const { return y; };
  Type GetZ(void) const { return z; };

  void SetX(Type t)   { x   = t; };
  void SetY(Type t)   { y   = t; };
  void SetZ(Type t)   { z   = t; };

  void Set(Type a,Type b,Type c)
  {
    x = a;
    y = b;
    z = c;
  };

  void Zero(void)
  {
    x = y = z = 0;
  };

// return -(*this).
	Vector3d negative(void) const
	{
		Vector3d result;
		result.x = -x;
		result.y = -y;
		result.z = -z;
		return result;
	}

	Type Magnitude(void) const
	{
		return Type(sqrtf(x * x + y * y + z * z));
	};

  void Lerp(const Vector3d<Type>& from,const Vector3d<Type>& to,float slerp)
  {
    *this = to-from; // delta on all 3 axis
    *this*=slerp;    // times interpolant distance.
    *this+=from;     // plus source
  };

  // Highly specialized interpolate routine.  Will compute the interpolated position
  // shifted forward or backwards along the ray defined between (from) and (to).
  // Reason for existance is so that when a bullet collides with a wall, for
  // example, you can generate a graphic effect slightly *before* it hit the
  // wall so that the effect doesn't sort into the wall itself.
  void Interpolate(const Vector3d<float> &from,const Vector3d<float> &to,float offset)
  {
    x = to.x-from.x;
    y = to.y-from.y;
    z = to.z-from.z;
    float d = sqrtf( x*x + y*y + z*z );
    float recip = 1.0f / d;
    x*=recip;
    y*=recip;
    z*=recip; // normalize vector
    d+=offset; // shift along ray
    x = x*d + from.x;
    y = y*d + from.y;
    z = z*d + from.z;
  };


/** Computes the reflection vector between two vectors.*/
  void Reflection(const Vector3d<Type> &a,const Vector3d<Type> &b)// compute reflection vector.
  {
/* original
  	Vector c;
  	Vector d;
  	Vector e;

		float dot = 2.0f*a->dot_product(b);

		c.x = b->x*dot;
		c.y = b->y*dot;
		c.z = b->z*dot;

		d.x = c.x - a->x;
		d.y = c.y - a->y;
		d.z = c.z - a->z;

		x = -d.x;
		y = -d.y;
		z = -d.z;
*/
    //converted to fit our Vector class
  	Vector3d<float> c;
  	Vector3d<float> d;

		float dot = a.Dot(b) * 2.0f;

		c = b * dot;

		d = c - a;

		x = -d.x;
		y = -d.y;
		z = -d.z;
  };

  void AngleAxis(Type angle,const Vector3d<Type>& axis)
  {
    x = axis.x*angle;
    y = axis.y*angle;
    z = axis.z*angle;
  };

  Type Length(void) const          // length of vector.
  {
    return Type(sqrt( x*x + y*y + z*z ));
  };

  Type Length2(void) const         // squared distance, prior to square root.
  {
    Type l2 = x*x+y*y+z*z;
    return l2;
  };

  Type Distance(const Vector3d<Type> &a) const   // distance between two points.
  {
    Vector3d<Type> d(a.x-x,a.y-y,a.z-z);
    return d.Length();
  }

  Type DistanceXY(const Vector3d<Type> &a) const
  {
	float dx = a.x - x;
	float dy = a.y - y;
    float dist = dx*dx + dy*dy;
	return dist;
  }

  Type Distance2(const Vector3d<Type> &a) const  // squared distance.
  {
    float dx = a.x - x;
    float dy = a.y - y;
    float dz = a.z - z;
    return dx*dx + dy*dy + dz*dz;
  };

  Type Partial(const Vector3d<Type> &p) const
  {
    return (x*p.y) - (p.x*y);
  }

  Type Area(const Vector3d<Type> &p1,const Vector3d<Type> &p2) const
  {
    Type A = Partial(p1);
    A+= p1.Partial(p2);
    A+= p2.Partial(*this);
    return A*0.5f;
  }

  Type Normalize(void)       // normalize to a unit vector, returns distance.
  {
    Type l = Length(); // get length.
    if ( l != 0 )
    {
      x/=l;
      y/=l;
      z/=l;
    }
    else
    {
      x = y = z = 0;
    }
    return l;
  };

  Type Dot(const Vector3d<Type> &a) const        // computes dot product.
  {
		return (x * a.x + y * a.y + z * a.z );
  };

  void Cross(const Vector3d<Type> &a,const Vector3d<Type> &b)  // cross two vectors result in this one.
  {
    x = a.y*b.z - a.z*b.y;
    y = a.z*b.x - a.x*b.z;
    z = a.x*b.y - a.y*b.x;
  };

  /******************************************/
  // Check if next edge (b to c) turns inward
  //
  //    Edge from a to b is already in face
  //    Edge from b to c is being considered for addition to face
  /******************************************/
  bool Concave(const Vector3d<float>& a,const Vector3d<float>& b)
  {
  	float vx,vy,vz,wx,wy,wz,vw_x,vw_y,vw_z,mag,nx,ny,nz,mag_a,mag_b;

  	wx = b.x - a.x;
  	wy = b.y - a.y;
  	wz = b.z - a.z;

  	mag_a = (float) core::squareroot((wx * wx) + (wy * wy) + (wz * wz));

  	vx = x - b.x;
  	vy = y - b.y;
  	vz = z - b.z;

  	mag_b = (float) sqrtf((vx * vx) + (vy * vy) + (vz * vz));

  	vw_x = (vy * wz) - (vz * wy);
  	vw_y = (vz * wx) - (vx * wz);
  	vw_z = (vx * wy) - (vy * wx);

  	mag = (float) sqrtf((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

  	// Check magnitude of cross product, which is a sine function
  	// i.e., mag (a x b) = mag (a) * mag (b) * sin (theta);
  	// If sin (theta) small, then angle between edges is very close to
  	// 180, which we may want to call a concavity.	Setting the
  	// CONCAVITY_TOLERANCE value greater than about 0.01 MAY cause
  	// face consolidation to get stuck on particular face.	Most meshes
  	// convert properly with a value of 0.0

  	if (mag/(mag_a*mag_b) <= 0.0f )	return true;

    mag = 1.0f / mag;

  	nx = vw_x * mag;
  	ny = vw_y * mag;
  	nz = vw_z * mag;

  	// Dot product of tri normal with cross product result will
  	// yield positive number if edges are convex (+1.0 if two tris
  	// are coplanar), negative number if edges are concave (-1.0 if
  	// two tris are coplanar.)

  	mag = ( x * nx) + ( y * ny) + ( z * nz);

  	if (mag > 0.0f ) return false;

  	return(true);
  };

  bool PointTestXY(const Vector3d<float> &i,const Vector3d<float> &j) const
  {
    if (((( i.y <= y ) && ( y  < j.y )) ||
         (( j.y <= y ) && ( y  < i.y ))) &&
          ( x < (j.x - i.x) * (y - i.y) / (j.y - i.y) + i.x)) return true;
    return false;
  }

  // test to see if this point is inside the triangle specified by
  // these three points on the X/Y plane.
  bool PointInTriXY(const Vector3d<float> &p1,
                  const Vector3d<float> &p2,
                  const Vector3d<float> &p3) const
  {
    float ax  = p3.x - p2.x;
    float ay  = p3.y - p2.y;
    float bx  = p1.x - p3.x;
    float by  = p1.y - p3.y;
    float cx  = p2.x - p1.x;
    float cy  = p2.y - p1.y;
    float apx = x - p1.x;
    float apy = y - p1.y;
    float bpx = x - p2.x;
    float bpy = y - p2.y;
    float cpx = x - p3.x;
    float cpy = y - p3.y;

    float aCROSSbp = ax*bpy - ay*bpx;
    float cCROSSap = cx*apy - cy*apx;
    float bCROSScp = bx*cpy - by*cpx;

    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
  };

  // test to see if this point is inside the triangle specified by
  // these three points on the X/Y plane.
  bool PointInTriYZ(const Vector3d<float> &p1,
                  const Vector3d<float> &p2,
                  const Vector3d<float> &p3) const
  {
    float ay  = p3.y - p2.y;
    float az  = p3.z - p2.z;
    float by  = p1.y - p3.y;
    float bz  = p1.z - p3.z;
    float cy  = p2.y - p1.y;
    float cz  = p2.z - p1.z;
    float apy = y - p1.y;
    float apz = z - p1.z;
    float bpy = y - p2.y;
    float bpz = z - p2.z;
    float cpy = y - p3.y;
    float cpz = z - p3.z;

    float aCROSSbp = ay*bpz - az*bpy;
    float cCROSSap = cy*apz - cz*apy;
    float bCROSScp = by*cpz - bz*cpy;

    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
  };


  // test to see if this point is inside the triangle specified by
  // these three points on the X/Y plane.
  bool PointInTriXZ(const Vector3d<float> &p1,
                  const Vector3d<float> &p2,
                  const Vector3d<float> &p3) const
  {
    float az  = p3.z - p2.z;
    float ax  = p3.x - p2.x;
    float bz  = p1.z - p3.z;
    float bx  = p1.x - p3.x;
    float cz  = p2.z - p1.z;
    float cx  = p2.x - p1.x;
    float apz = z - p1.z;
    float apx = x - p1.x;
    float bpz = z - p2.z;
    float bpx = x - p2.x;
    float cpz = z - p3.z;
    float cpx = x - p3.x;

    float aCROSSbp = az*bpx - ax*bpz;
    float cCROSSap = cz*apx - cx*apz;
    float bCROSScp = bz*cpx - bx*cpz;

    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
  };

//private:

  Type x;
  Type y;
  Type z;
};

/*
template <class Type> class Vector2d
{
public:
  Vector2d(void) { };  // null constructor, does not inialize point.

  Vector2d(const Vector2d &a) // constructor copies existing vector.
  {
    x = a.x;
    y = a.y;
  };

  Vector2d(Type a,Type b) // construct with initial point.
  {
    x = a;
    y = b;
  };


  Vector2d & operator+=(const Vector2d &a) // += operator.
  {
    x+=a.x;
    y+=a.y;
    return *this;
  };

  Vector2d & operator-=(const Vector2d &a)
  {
    x-=a.x;
    y-=a.y;
    return *this;
  };

  Vector2d & operator*=(const Vector2d &a)
  {
    x*=a.x;
    y*=a.y;
    return *this;
  };

  Vector2d & operator/=(const Vector2d &a)
  {
    x/=a.x;
    y/=a.y;
    return *this;
  };

  bool operator==(const Vector2d<Type> &a) const
  {
    if ( a.x == x && a.y == y ) return true;
    return false;
  };

  bool operator!=(const Vector2d &a) const
  {
    if ( a.x != x || a.y != y ) return true;
    return false;
  };

  Vector2d operator+(Vector2d a) const
  {
    a.x+=x;
    a.y+=y;
    return a;
  };

  Vector2d operator-(Vector2d a) const
  {
    a.x = x-a.x;
    a.y = y-a.y;
    return a;
  };

  Vector2d operator - (void) const
  {
    return negative();
  };

  Vector2d operator*(Vector2d a) const
  {
    a.x*=x;
    a.y*=y;
    return a;
  };

	Vector2d operator*(Type c) const
  {
    Vector2d<Type> a;

		a.x = x * c;
		a.y = y * c;

    return a;
  };

  Vector2d operator/(Vector2d a) const
  {
    a.x = x/a.x;
    a.y = y/a.y;
    return a;
  };


	Type Dot(const Vector2d<Type> &a) const        // computes dot product.
  {
		return (x * a.x + y * a.y );
  };

  Type GetX(void) const { return x; };
  Type GetY(void) const { return y; };

  void SetX(Type t) { x   = t; };
  void SetY(Type t) { y   = t; };

  void Set(Type a,Type b)
  {
    x = a;
    y = b;
  };

  void Zero(void)
  {
    x = y = z = 0;
  };

	Vector2d negative(void) const
	{
		Vector2d result;
		result.x = -x;
		result.y = -y;
		return result;
	}

	Type magnitude(void) const
	{
		return (Type) sqrtf(x * x + y * y );
	}

  void Reflection(Vector2d &a,Vector2d &b); // compute reflection vector.

  Type Length(void) const          // length of vector.
  {
    return Type(sqrtf( x*x + y*y ));
  };


  Type Length2(void)        // squared distance, prior to square root.
  {
    return x*x+y*y;
  }

  Type Distance(const Vector2d &a) const   // distance between two points.
  {
    Type dx = a.x - x;
    Type dy = a.y - y;
    Type d  = dx*dx+dy*dy;
    return sqrtf(d);
  };

  Type Distance2(Vector2d &a) // squared distance.
  {
    Type dx = a.x - x;
    Type dy = a.y - y;
    return dx*dx + dy *dy;
  };

  void Lerp(const Vector2d<Type>& from,const Vector2d<Type>& to,float slerp)
  {
    x = to.x - from.x;
    y = to.y - from.y;
    x*=slerp;
    y*=slerp;
    x+=from.x;
    y+=from.y;
  };


  void Cross(const Vector2d<Type> &a,const Vector2d<Type> &b)  // cross two vectors result in this one.
  {
    x = a.y*b.x - a.x*b.y;
    y = a.x*b.x - a.x*b.x;
  };

	Type Normalize(void)       // normalize to a unit vector, returns distance.
  {
    Type l = Length();
    if ( l != 0 )
    {
      l = 1.0f / l;
      x*=l;
      y*=l;
    }
    else
    {
      x = y = 0;
    }
    return l;
  };


//private:
  Type x;
  Type y;
};

typedef std::vector< Vector3d<float> > Vector3dVector;
typedef std::vector< Vector2d<float> > Vector2dVector;

template <class Type> Vector3d<Type> operator * (Type s, const Vector3d<Type> &v )
      { Vector3d <Type> Scaled(v.x*s, v.y*s, v.z*s);
        return(Scaled); };

template <class Type> Vector2d<Type> operator * (Type s, const Vector2d<Type> &v )
      { Vector2d <Type> Scaled(v.x*s, v.y*s);
        return(Scaled); };
*/



class LightMapVertex
{
public:
  LightMapVertex(void)
  {
  };

  LightMapVertex(float x,float y,float z,float u1,float v1,float u2,float v2)
  {
    mPos.Set(x,y,z);
    mTexel1.set(u1,v1);
    mTexel2.set(u2,v2);
  }


  void GetPos(Vector3d<float> &pos) const { pos = mPos; };
  const Vector3d<float>& GetPos(void) const { return mPos; };

  float GetX(void) const { return mPos.x; };
  float GetY(void) const { return mPos.y; };
  float GetZ(void) const { return mPos.z; };

  void Lerp(const LightMapVertex &a,const LightMapVertex &b,float p)
  {
    mPos.Lerp(a.mPos,b.mPos,p);
//    mTexel1.Lerp(a.mTexel1,b.mTexel1,p);
 //   mTexel2.Lerp(a.mTexel2,b.mTexel2,p);
  };

  void Set(int index,const float *pos,const float *texel1,const float *texel2)
  {
    const float * p = &pos[index*3];

    const float * tv1 = &texel1[index*2];
    const float * tv2 = &texel2[index*2];

    mPos.x     = p[0];
    mPos.y     = p[1];
    mPos.z     = p[2];
/*    mTexel1.x  = tv1[0];
    mTexel1.y  = tv1[1];
    mTexel2.x  = tv2[0];
    mTexel2.y  = tv2[1];*/
  };

  Vector3d<float> mPos;
  core::vector2df mTexel1;
  core::vector2df mTexel2;

};


class PatchSurface
{
public:
  PatchSurface(const LightMapVertex *control_points, int controlx,int controly);
  ~PatchSurface(void);

  const LightMapVertex * GetVerts(void) const { return mPoints; };
  const unsigned short * GetIndices(void) const { return mIndices; };
  int GetIndiceCount(void) const { return mCount; };

//private:
  bool FindSize(int controlx,int controly,const LightMapVertex *cp,int &sizex,int &sizey) const;
  void FillPatch(int controlx,int controly,int sizex,int sizey,LightMapVertex *points);
  void FillCurve(int numcp, int size, int stride,LightMapVertex *p);

  int FindLevel(const core::vector3df &cv0,
                            const core::vector3df &cv1,
                            const core::vector3df &cv2) const;

  int                  mVertCount;
  int                  mCount;        // number of indexes/
  LightMapVertex      *mPoints;       // vertices produced.
  unsigned short      *mIndices;      // indices into those vertices
	core::vector3df           *mNormals;
};


struct patch_6dx
	{
	int   width,height; // number of control points in x,y direction. Typically 3x3.
	int   contents;
	int   flags;
	float value;

  core::matrix4 *mat;
  core::vector3df mins,maxs;
	int tricount;
	int vertcount;      // current
  int maxverts;       // maximum number of verts.
	int x_subdivision;  // number of triangles generated in one dimension. Ie, 10 units = 10x10 vertexes.
  int y_subdivision;

//	D3DVERTEX Control[4][4]; // one for each control point in the patch. usually 3x3 but can be 4x4
	//D3DLVERTEX *Mesh;         // one for each actual vertex in the world.
  unsigned short *Indexes; // normals already computed.
  rmip_t *mip;

	LightMapVertex *cp;
  PatchSurface *surface;
  patch_6dx(patch_6dx &p);
  patch_6dx();
 ~patch_6dx();
  void Build();
  void Build(int steps);
  void Build(int subdivide_x,int subdivide_z);
	};
