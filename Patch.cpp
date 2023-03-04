#include "Patch.h"

patch_6dx::patch_6dx()
  {
	surface  = NULL;
//  Mesh     = NULL;
  //Indexes  = NULL;
	cp       = NULL;
  maxverts = 0;
  }

patch_6dx::~patch_6dx()
  {
	if (surface) delete surface;
	if (cp)      delete [] cp;
//  if (Mesh)    delete [] Mesh;
  //if (Indexes) delete [] Indexes;
  }

double BezierBlend(int k,double mu,int n)
	{
  int nn,kn,nkn;
  double blend=1;

  nn = n;
  kn = k;
  nkn = n - k;

	while (nn >= 1)
		{
     blend *= nn;
     nn--;
     if (kn > 1)
			 {
       blend /= (double)kn;
       kn--;
       }
     if (nkn > 1)
			 {
       blend /= (double)nkn;
       nkn--;
       }
		}
  if (k > 0)   blend *= pow(mu,(double)k);
  if (n-k > 0) blend *= pow(1-mu,(double)(n-k));

  return(blend);
	}

/*static inline float fmin(float val1,float val2)
  {
  if (val1 < val2) return val1;
  return val2;
  }

static inline float fmax(float val1,float val2)
  {
  if (val1 > val2) return val1;
  return val2;
  }*/

struct bernBasis_t
{
   static float Get(int base, int i, float t);
   static float GetDeriv(int base, int i, float t);
};

// Basis functions, should pre-compute
float bernBasis_t::Get(int base, int i, float t)
{
   float v = (1.0f - t);

   if (base == 2)
   {
      if (i == 0)
         return v * v;
      else if (i == 1)
         return 2 * t * v;
      else if (i == 2)
         return t * t;
   }
   else if (base == 3)
   {
      if (i == 0)
         return v * v * v;
      else if (i == 1)
         return 3 * t * v * v;
      else if (i == 2)
         return 3 * t * t * v;
      else if (i == 3)
         return t * t * t;
   }
   return 0;

}

// Derivatives of the basis functions, should pre-compute
float bernBasis_t::GetDeriv(int base, int i, float t)
  {
   float v = (1.0f - t);

   if (base == 2)
   {
      if (i == 0)
         return (2 * t) - 2;
      else if (i == 1)
         return 1 - (2 * t);
      else if (i == 2)
         return 2 * t;
   }
   else if (base == 3)
   {
      if (i == 0)
         return -3 * v * v;
      else if (i == 1)
         return 3 - (12 * t) + (9 * t * t);
      else if (i == 2)
         return (6 * t) - (9 * t * t);
      else if (i == 3)
         return 3 * t * t;
   }
   return 0;
  }
/*
static inline void CalcPlane2(D3DVERTEX *v0, D3DVERTEX *v1, D3DVERTEX *v2,D3DVERTEX *normal)
    {
    D3DVERTEX *u, *v;
    static D3DVERTEX f[3];

    normal->x = 0;
    normal->y = 0;
    normal->z = 0;

    f[0] = *v0;
    f[1] = *v1;
    f[2] = *v2;

    u = &f[2];
    v = &f[0];

    for (int i = 0; i < 3; i++, u = v++)
      {
      normal->x += (u->y - v->y) * (u->z + v->z);
      normal->y += (u->z - v->z) * (u->x + v->x);
      normal->z += (u->x - v->x) * (u->y + v->y);
      }
    }
*/
void patch_6dx::Build(int subdivide_x,int subdivide_z)
	{
  // gen the normals for each plane.
  /*
	for (int i=0;i<tricount;i++)
		{
		static D3DVERTEX normal;
		D3DVERTEX *v1, *v2, *v3;

		int tribase = (i*3);

		v1 = Mesh+Indexes[tribase++];
		v2 = Mesh+Indexes[tribase++];
		v3 = Mesh+Indexes[tribase++];
    CalcPlane2(v3,v1,v2,&normal);
		// CalcPlane2(v+3,v,v+2,&gp->planes[0]);
		// get each vertex of the triangle.
		// calc the plane normal.
	  // add it to the normal for each vertex
		v1->nx += normal.x;
		v1->ny += normal.y;
		v1->nz += normal.z;

		v2->nx += normal.x;
		v2->ny += normal.y;
		v2->nz += normal.z;

		v3->nx += normal.x;
		v3->ny += normal.y;
		v3->nz += normal.z;

		} */

	// go through the vertexes, and normalize the normals.
/*	for (i=0;i<vertcount;i++)
		{
		D3DVERTEX *src = Mesh+i;
    D3DVECTOR *v = (D3DVECTOR *) src;
		Normalize(*v);
		}*/
	}


#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)
#define MAXMESHLEVEL 4
#define MINDIST (0.4f*0.4f)

PatchSurface::PatchSurface(const LightMapVertex *cp,
                           int controlx,
                           int controly)
{
 /* int sizex,sizey;
  FindSize(controlx,controly,cp,sizex,sizey);

  mVertCount = sizex*sizey;
  mPoints = new LightMapVertex[mVertCount];
	mNormals = new D3DVECTOR[mVertCount];

  int stepx = (sizex-1) / (controlx-1);
  int stepy = (sizey-1) / (controly-1);
  const LightMapVertex *control = cp;

  for (int y=0; y<sizey; y+=stepy)
  {
    for (int x=0; x<sizex; x+=stepx)
    {
      int p = y*sizex+x;
      mPoints[p] = *control++;
    }
  }


  FillPatch(controlx,controly,sizex,sizey,mPoints);

  mCount = (sizex-1)*(sizey-1)*6;

  if ( 1 )
  {
    mIndices = new unsigned short[mCount];
    unsigned short *foo = mIndices;
    for (int y=0; y < sizey-1; ++y)
    {
    	for (int x = 0; x < sizex-1; ++x)
    	{
        *foo++ = y*sizex+x;
        *foo++ = y*sizex+x+1;
        *foo++ = (y+1)*sizex+x;

        *foo++ = y*sizex+x+1;
        *foo++ = (y+1)*sizex+x+1;
        *foo++ = (y+1)*sizex+x;
    	}
    }
  }*/
}

PatchSurface::~PatchSurface(void)
{
  delete mIndices;
  delete mPoints;
}

void patch_6dx::Build()
	{
	surface = new PatchSurface(cp,width,height);

	/*
	// fill in my stuff?
  int newverts  = surface->mVertCount;
	tricount      = surface->mCount/3; //(steps*steps) *2;
  if (newverts > maxverts)
    {
    maxverts = newverts;
    if (Mesh)    delete [] Mesh;
    Mesh = new D3DLVERTEX[newverts];
    }
  mins.x = 99999999.0f;
  mins.y = 99999999.0f;
  mins.z = 99999999.0f;
  maxs.x = 99999999.0f;
  maxs.y = 99999999.0f;
  maxs.z = 99999999.0f;

  vertcount     = newverts;
  for (int i=0;i<vertcount;i++)
    {
    LightMapVertex *lmv   =  surface->mPoints+i;
    D3DLVERTEX      *destv =  Mesh+i;
    destv->x = lmv->mPos.x;
    destv->y = lmv->mPos.y;
    destv->z = lmv->mPos.z;
    destv->tu = lmv->mTexel1.x;
    destv->tv = lmv->mTexel1.y;
    destv->color = 0xffffffff;
    destv->specular = 0;
    //destv->nx = 0.0f;
    //destv->ny = 1.0f;
    //destv->nz = 0.0f;
    maxs.x = fmax(destv->x,maxs.x);
    maxs.y = fmax(destv->y,maxs.y);
    maxs.z = fmax(destv->z,maxs.z);

    mins.x = fmin(destv->x,mins.x);
    mins.y = fmin(destv->y,mins.y);
    mins.z = fmin(destv->z,mins.z);
    }
  Indexes = surface->mIndices;

  */
  }


bool PatchSurface::FindSize(int controlx,int controly,const LightMapVertex *cp,int &sizex,int &sizey) const
{
  /* Find non-coincident pairs in u direction */

  bool found=false;
/*
  const LightMapVertex *a=0;
  const LightMapVertex *b=0;

  for (int v=0; v <controly; v++)
  {
	  for (int u=0; u < controlx; u+=2)
  	{

	    a = &cp[v * controlx + u];
	    b = &cp[v * controlx + u + 2];

      if ( a->mPos.x != b->mPos.x ||
           a->mPos.y != b->mPos.y ||
           a->mPos.z != b->mPos.z )
	    {
    		found = true;
	    	break;
	    }
   	}
  	if (found) break;
  }

  if (!found)
  {
    printf("Bad mesh control points\n");
    return false;
  }


  int levelx = FindLevel(a[0].mPos,a[1].mPos,b[0].mPos);
  sizex = (LEVEL_WIDTH(levelx) - 1) * ((controlx-1) / 2) + 1;


  for (int u=0; u <controlx; v++)
  {
	  for (int v=0; v < controly; v+=2)
  	{

	    a = &cp[v * controlx + u];
	    b = &cp[ ((v+2) * controlx) + u ];

      if ( a->mPos.x != b->mPos.x ||
           a->mPos.y != b->mPos.y ||
           a->mPos.z != b->mPos.z )
	    {
    		found = true;
	    	break;
	    }
   	}
  	if (found) break;
  }

  if (!found)
  {
    printf("Bad mesh control points\n");
    return false;
  }


  int levely = FindLevel(a[0].mPos,a[1].mPos,b[0].mPos);
  sizey = (LEVEL_WIDTH(levely) - 1) * ((controly-1) / 2) + 1;
*/
  return true;
}


int PatchSurface::FindLevel(const core::vector3df &cv0,
                            const core::vector3df &cv1,
                            const core::vector3df &cv2) const
{
  int level;
  /*
  Vector3d<float> a,b,dist;

  Vector3d<float> v0 = cv0;
  Vector3d<float> v1 = cv1;
  Vector3d<float> v2 = cv2;

  for (level=0; level <MAXMESHLEVEL-1; level++)
  {

    a.Lerp(v0,v1,0.5f);
    b.Lerp(v1,v2,0.5f);
    v2.Lerp(a,b,0.5f);


    dist = v2-v1;

    float dist2 = dist.x*dist.x + dist.y*dist.y + dist.z*dist.z;
    if ( dist2 < MINDIST ) break;


    v1 = a;
  }*/

  return level;
}

void PatchSurface::FillPatch(int controlx,int controly,int sizex,int sizey,LightMapVertex *p)
{
  int stepx = (sizex-1) / (controlx-1);
  for (int u = 0; u < sizex; u += stepx)
  {
  	FillCurve(controly, sizey,sizex,p+u);
  }
  for (int v = 0; v < sizey; v++)
  {
  	FillCurve(controlx, sizex, 1, p + v * sizex);
  }
}

void PatchSurface::FillCurve(int numcp, int size, int stride,LightMapVertex *p)
{
  int step, halfstep, i, mid;
  LightMapVertex a,b;

  step = (size-1) / (numcp-1);

  while (step > 0)
  {
	  halfstep = step / 2;
  	for (i=0; i < size-1; i += step*2)
	  {
	    mid = (i+step)*stride;
      a.Lerp(p[i*stride],p[mid],0.5f);
      b.Lerp(p[mid],p[(i+step*2)*stride],0.5f);
      p[mid].Lerp(a,b,0.5f);

	    if (halfstep > 0)
	    {
		    p[(i+halfstep)*stride]   = a;
    		p[(i+3*halfstep)*stride] = b;
	    }
	  }
  	step /= 2;
  }
}
