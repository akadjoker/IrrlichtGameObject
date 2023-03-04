
#pragma once
#include "irrlicht.h"
#include "Lists.h"
#include "Patch.h"

using namespace irr;
using namespace scene;
using namespace video;
#define MAX_CLIPBRUSHES         2048
#define WORLDFLAGS_NOLIGHTMAPS  0x0001
#define WORLDFLAGS_ALLLIGHTMAPS 0x0002

/*#define CLIPFLAGS_INCLUDEACTORS      0x0001
#define CLIPFLAGS_INCLUDEPLAYERCLIP  0x0002
#define CLIPFLAGS_INCLUDEMONSTERCLIP 0x0004
#define CLIPFLAGS_INCLUDEWATER       0x0008
#define CLIPFLAGS_INCLUDEPORTALS     0x0010

// Combos
#define CLIPFLAGS_CAMERA             0x0003
#define CLIPFLAGS_ACTOR              0x0005
#define CLIPFLAGS_INCLUDEBOUNDS      0x000E*/
#define MAX_GROUPS  2048

// by default, you can not pass through a brush, and it is visible.
#define INVISIBLE               0x0001                          // make the brushes invisible.
#define INSUBSTANTIAL           0x0002                          // make the brushes insubstantial.
#define TRIGGER                 0x0004                          // insubstantial, but include anyway for triggerbox checking.
#define VISTRIGGER              0x0008                          // visible -- ie, needs light.
#define CLIPFLAGS_ALL           0x0007                          // include all top level groups. For checking what group someone is in.
#define FORCEBRUSHES_INVISIBLE  0x0010
#define FORCEBRUSHES_NOBLOCKING 0x0020
#define PICK_MAPS               0x0001
#define PICK_ACTORS             0x0002
#define PICK_GRID               0x0004
#define PICK_ALL                0xffff

// #define COLLISION              0x0002
// #define COLLISION_MONSTERBLOCK 0x0004
#define SURFACE_PORTAL  0x0001
#define SURFACE_NODRAW  0x0002
#define SURF_LIGHT      0x1                                     // value holds light. Only used for this face and flat shading.
#define SURF_SLICK      0x2                                     // effects game physics
#define SURF_DBLSIDE    0x4                                     // don't draw, but add to skybox
#define SURF_LIGHTMAP   0x8                                     // face has a lightmap.
#define SURF_COLORKEY   0x10                                    //
#define SURF_TRANS66    0x20                                    // value holds the amount of transparency
#define SURF_FLOWING    0x40                                    // scroll towards angle
#define SURF_NODRAW     0x80                                    // Don't draw this one.
#define SURF_MIRROR     0x100                                   // Gouraud shaded - no lightmaps.
#define SURF_PORTAL     0x200                                   // It's a portal.

//#define SURF_MIRROR     0x400
// #define SURF_PORTAL     0x1000
#define V2_CONTENTS_SOLID         1                             // an eye is never valid in a solid
#define V2_CONTENTS_WINDOW        2                             // translucent, but not watery
#define V2_CONTENTS_AUX           4
#define V2_CONTENTS_LAVA          8
#define V2_CONTENTS_SLIME         16
#define V2_CONTENTS_WATER         32
#define V2_CONTENTS_MIST          64
#define V2_CONTENCT_ADDITIVE      128
#define V2_LAST_VISIBLE_CONTENTS  64

// remaining contents are non-visible, and don't eat brushes
#define V2_CONTENTS_AREAPORTAL  0x8000
#define V2_CONTENTS_PLAYERCLIP  0x10000
#define V2_CONTENTS_MONSTERCLIP 0x20000

// currents can be added to any other contents, and may be mixed
#define V2_CONTENTS_CURRENT_0     0x40000
#define V2_CONTENTS_CURRENT_90    0x80000
#define V2_CONTENTS_CURRENT_180   0x100000
#define V2_CONTENTS_CURRENT_270   0x200000
#define V2_CONTENTS_CURRENT_UP    0x400000
#define V2_CONTENTS_CURRENT_DOWN  0x800000
#define V2_CONTENTS_ORIGIN        0x1000000                     // removed before bsping an entity
#define V2_CONTENTS_MONSTER       0x2000000                     // should never be on a brush, only in game
#define V2_CONTENTS_DEADMONSTER   0x4000000
#define V2_CONTENTS_DETAIL        0x8000000                     // brushes to be added after vis leafs
#define V2_CONTENTS_TRANSLUCENT   0x10000000                    // auto set if any surface has trans
#define V2_CONTENTS_LADDER        0x20000000
#define BRUSH_GOURAUD             0x2
#define BRUSH_FLATSHADE           0x4
#define BRUSH_LIGHTMAP            0x8



typedef float vec2_t	[3];
typedef float vec3_t	[3];
typedef float vec6_t	[3];
typedef struct
	{
	float u,v;
	} uvvect_t;

struct vec3_uv
  {
  float vector[3];
  float uv[2];
  };

typedef struct
{
  vec3_t		p;
  u8   		ccodes;
  float 		fx, fy;
  float  		u, v;
} point3_t;

typedef struct
{
	vec3_t	world;
	vec3_t	view;
	vec2_t	screen;
  float   inv_z;
	int			clip_flag;
	int			flag;
} point_t;


struct rmip_t
  {
  string                texname;
  string                name;
  bool                  additive;         // only contains additive textures - all faces are additive.

  int                   currmip;
  int                   usertex;
  int                   facecount;
/*
  AlphaContainer        *ac;              // the user texture that this mip is on. if it is one.
  LightmapContainer     *lmap;

  // int  framecounter;    // the frame that chain was created.
  TObjectList<anim_mip> textures;
  TList<face_6dx>       faces;
  TList<face_6dx>       alterfaces;
  TList<face_6dx>       alphafaces;
  TList<patch_6dx>      patches;*/
 // rmip_t();
 // ~rmip_t();
  };

struct readplane_t
  {
  core::vector3df    points[3];
  //core::p  plane;
  char      texture[32];
  int       xoff;
  int       yoff;
  float     rotation;
  float     xscale;
  float     yscale;
  float     value;
  int       contents;
  int       surface;
  core::vector3df    *facepoints;
  core::vector3df    *savepoints;
  int       facepointcount;
  rmip_t    *mip;
  };
struct readbrush_t
  {
  int         planecount;
  readplane_t *rplanes;
  int         brushnum;
  };
struct brush_6dx;
struct entgroup_6dx;
struct qle_trigger_t;


struct vertex3
  {
  float x, y, z;
  int color;

  //  DWORD specular;
  float tu1, tv1;
  float tu2, tv2;
  };
struct uvcoord
  {
  float u;
  float v;
  };
struct dfbox_t                      // Bounding Box, float values
  {
  float min[3];                     // minimum values of X,Y,Z
  float max[3];                     // maximum values of X,Y,Z
  };

// Definition of one plane
struct dplane_t
  {
  float normal[3];                  // Plane normal
  float dist;                       // Distance to origin
  int   type;                       // PLANE_X - PLANE_ANYZ
  };
struct shortface
  {
  short     vertexcount;            // the number of vertexes. // Short...
  vertex3   *vertexes;              // the vertexes that make up this face. Clockwise.
  brush_6dx *brush;
  shortface();
  ~shortface();
  };


struct keypair
  {
  char  keyword[32];                                            // the type
  char  value[128];                                             // the value
  };
struct readent_t
  {
  char    classname[32];                                        // pull the
  int     keypaircount;                                         // the number of keypairs.
  keypair *keypairs;                                            // an array of keypairs - keywords/values.
  core::vector3df  origin;                                               // usually at 0,0,0 by default. then needs to be translated.
  int     group;                                                // the group that this is in.
  };

struct face_6dx
  {
  short             vertexcount;    // the number of vertexes. // Short...
  vertex3           *vertexes;      // the vertexes that make up this face. Clockwise.
  brush_6dx         *brush;

  //int        facenumber;
  dplane_t          plane;          // the plane that defines this face. Straight from the file.
//  rmip_t            *mip;           // The texture for this face.
  uvcoord           *uv;
  short             lm_iwidth;      // short...
  short             lm_iheight;     // short...
  short             lmx, lmy;       // short
  short             stylecount;     // stylecount
  short             properties;     // like SURFACE_PORTAL.    // Short...
  int               lightoffset;
  int               lightpos;
  u8              lightstyles[5]; // type of light
  u8              faceflags;
  u8              minr;
  u8              ming;
  u8              minb;
  u8              lightaxis1;
  u8              lightaxis2;
  float             lightminu;
  float             lightminv;
  unsigned long     lightsequencer;
  //LightmapContainer *lightmap;
  u8              tempignore : 1; // normally false;
  u8              moved : 1;
  u8              needlight : 1;
  u8              selected : 1;

  // void     *collider;
  face_6dx          *cloneparent;   // the cloneparent of this face, if it's on the Instance.
  core::matrix4         *mat;
 // ~face_6dx();
 // face_6dx();
 // face_6dx(face_6dx & f);
  };
struct brush_6dx
  {
  unsigned int  facecount;          // the number of faces in this brush.
  unsigned int  totalfaces;         // facecount + number of duplicated faces.
  dplane_t      *worldplanes;
  face_6dx      *faces;             // the faces in this brush.
  core::vector3df        mins;               // the min extents of this brush - used for culling.
  core::vector3df        maxs;               // The max extents of this brush - used for culling.
  core::vector3df        center;
  int           flags;              // like NOT_VISIBLE, etc.
  int           contents;           // like SOLID, WATER, etc.
  unsigned int  brushnumber;        // short
  int           surfaceproperties;  // all the faces surface properties or'd together.
  unsigned int  mastergroup;        // short         // the top level group number;
  entgroup_6dx  *parent;            // the parent group.
  unsigned int  renderframe;
  unsigned int  triggerserializer;
  //actor_6dx     *srcactor;
  u8          visible : 1;
  u8          blocking : 1;
  u8          tempignore : 1;
  u8          selected : 1;
  u8          actor : 1;          // set if we want actor_notify
  u8          clipbox : 1;        // a ClipBox
  u8          actorevt : 1;
  qle_trigger_t *trigger;

  // flags
//  brush_6dx();
////  brush_6dx(brush_6dx & b);
  //~brush_6dx();
  };

typedef struct
  {
  float roll;
  float pitch;
  float yaw;
  } angle_t;
enum ent_type { entity_corner, entity_actor, entity_trigger };
enum entgroup_class { clfunc_group, clfunc_portalgroup, clfunc_wall, cltrigger_box, clfunc_mover, clfunc_rotater, clfunc_rotatingdoor, clfunc_train };

class entity_t
  {
  public:
    core::vector3df origin;
    angle_t   angles;
    int       itemid;   // the "targetname" of this item.
    int       targetid; // the "target" of this item.
    float     radius;
    ent_type  enttype;
  };
struct qle_entity_t
  {
  core::vector3df origin;
  int       class_id;
   qle_entity_t(char *classname, float *orig)
  {
   // class_id = slClassnames.Add(classname);
  // VECTORCOPY(orig, origin);
  }

// Function
  qle_entity_t(void)
  {
  //class_id = slClassnames.Add("func_group");
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = 0;
  }

  };


struct entgroup_6dx : public qle_entity_t
  {
  void                      *objinterface;
  //qle_instance              *owner;           // owner of the trigger -- valid only for triggers on instances!!!
  core::vector3df                 mins;             // The Min extents of this group. used for culling.
  core::vector3df                 maxs;             // The Max extents of this group. Used for culling.
  core::vector3df                 minlight;
  u32                     color;
  u32                     specular;
  bool                      alternatecolor;
  int                       flags;            // like NOT_VISIBLE. This is all the brushes below it flags or'd together.
  int                       mastergroupno;
  int                       groupno;
  unsigned int              ClipFrame;
  unsigned int              RenderFrame;      // the frame that this was last rendered on. For use when rendering groups from qle instance.
  int                       targetname_id;
  int                       target_id;
  entgroup_class            classtype;

  //  vector<entgroup_6dx *>        Groups;         // the main group list.
  TObjectList<entgroup_6dx> Groups;           // list of brushes, so we don't have to go through the hierarchy.
  TObjectList<brush_6dx>    Brushes;          // list of brushes, so we don't have to go through the hierarchy.
  TObjectList<patch_6dx>    Patches;          // Patches.
  //TObjectList<light_6dx>    *Lights;          // only filled in if a master group.
  entgroup_6dx              *mastergroup;
  entgroup_6dx              *parent;          // the immediate parent group
  char                      *GetTarget(void);
  char                      *GetTargetname(void);
  //entgroup_6dx (void);                        //
 // entgroup_6dx(entgroup_6dx & eg);
 // entgroup_6dx (char *classname, float *origin);
 // virtual~entgroup_6dx();

  entgroup_6dx(char *classname, float *neworigin)
  {
  objinterface = NULL;
//  VectorClear(minlight);
 // VectorClear(mins);
  //VectorClear(maxs);
  flags = 0;
  ClipFrame = 0;
  RenderFrame = 0;
  origin[0] = neworigin[0];
  origin[1] = neworigin[1];
  origin[2] = neworigin[2];
  //class_id = slClassnames.Add(classname);
  mastergroup = NULL;
//  Lights = NULL;
  target_id = -1;
  targetname_id = -1;
  color = 0xffffffff;
  specular = 0xff202020;
  alternatecolor = false;
  }
  entgroup_6dx(void)
  {
  objinterface = NULL;
//  VectorClear(minlight);
//  VectorClear(mins);
 // VectorClear(maxs);
  origin[0] = 0.0f;
  origin[1] = 0.0f;
  origin[2] = 0.0f;
  class_id = 0;
  ClipFrame = 0;
  RenderFrame = 0;
  mastergroup = NULL;
//  Lights = NULL;
  flags = 0;
  classtype = clfunc_group;
  color = 0xffffffff;
  specular = 0xff202020;
  alternatecolor = false;
  }


  virtual entgroup_6dx *Clone(void)
    {
    return (new entgroup_6dx(*this));
    }
  };
