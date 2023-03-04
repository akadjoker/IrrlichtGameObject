#include <exception>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>


#include <SDL2/SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <cstdio>
#include <cstdlib>
#include "irrlicht.h"
#include "CVertexBuffer.h"
#include <string.h>
#include "Utils.h"
#include "Core.h"
#include "CColorConverter.h"

#include "CSkinnedMesh.h"
#include "ISkinnedMesh.h"
#include "SSkinMeshBuffer.h"

#include "matrix3.h"

#include "gdx.h"
#include "Lists.h"
#include "Patch.h"

using namespace irr;
using namespace scene;
using namespace video;



const unsigned int DISP_WIDTH = 1025;
const unsigned int DISP_HEIGHT = 720;
const char* TITLE=" Stencil test Batch";
const Uint8 *state=NULL;
Uint32 mouse_buttons;
int mouse_x;
int mouse_y;

irr::scene::Engine* engine;
int width, height;
float aspect;


// camera
//core::vector3df cameraPos   = core::vector3df(2700*2,255*2,2600*2);
//core::vector3df cameraFront = core::vector3df(2397*2,343*2,2700*2);


core::vector3df cameraPos   = core::vector3df(0,20,85);
core::vector3df cameraFront = core::vector3df(0,-4,0);

core::vector3df cameraUp    = core::vector3df(0.0f, 1.0f, 0.0f);


/*
camera->setPosition(core::vector3df(2700*2,255*2,2600*2));
	camera->setTarget(core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(42000.0f);
*/

bool firstMouse = true;
bool isMouseDown = false;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;



void processInput()
{

    float cameraSpeed = 400 * engine->GetFrameTime();
    if (state[SDL_SCANCODE_W])
    {
    cameraPos += cameraSpeed * cameraFront;
    }

    if (state[SDL_SCANCODE_S])
    {
     cameraPos -= cameraSpeed * cameraFront;
    }

    if (state[SDL_SCANCODE_A])
    {
    core::vector3df n = cameraFront.crossProduct(cameraUp).normalize();

    cameraPos -= n * cameraSpeed;
    }

    if (state[SDL_SCANCODE_D])
    {
      core::vector3df n = cameraFront.crossProduct(cameraUp).normalize();
      cameraPos += n * cameraSpeed;
    }



}

void window_size_callback(int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
    glViewport(0, 0, newWidth, newHeight);
    glScissor(0, 0, newWidth, newHeight);
    core::matrix4 pMat;
    pMat.buildProjectionMatrixPerspectiveFovRH(core::degToRad(fov),aspect, 0.1f, 42000.0f);
    engine->setProjectionMatrix(pMat);
    engine->setCameraFov(core::degToRad(fov));
    engine->setCameraAspect(aspect);

}
void mouse_callback( double xpos, double ypos)
{
	if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;


    if ((mouse_buttons & SDL_BUTTON_RMASK) != 0)
    {

    yaw += xoffset;
	pitch += yoffset;
    }



    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
	if (pitch < -89.0f)
        pitch = -89.0f;

	core::vector3df  front;


	front.X = cos(core::degToRad(yaw)) * cos(core::degToRad(pitch));
	front.Y = sin(core::degToRad(pitch));
	front.Z = sin(core::degToRad(yaw)) * cos(core::degToRad(pitch));
	cameraFront = front.normalize();

}




// Function
bool PlaneFromVertexes(face_6dx *f)
  {
  dplane_t  *pl = &f->plane;
  core::vector3df *v0, *v1, *v2, *normal;
/*
  // use 0,1,3 - to maximize probability that points don't lie on a straight line.
  if(f->vertexcount > 3)
    {
    v0 = (core::vector3df *) (f->vertexes + 2);
    v1 = (core::vector3df *) (f->vertexes + 1);
    v2 = (core::vector3df *) (f->vertexes + 0);
    }
  else
    {
    v0 = (core::vector3df *) (f->vertexes + 2);
    v1 = (core::vector3df *) (f->vertexes + 1);
    v2 = (core::vector3df *) (f->vertexes + 0);
    }


  core::plane3df plane=core::plane3df(v0,v1,v2);


  pl->normal[0]=plane.Normal.X;
  pl->normal[1]=plane.Normal.Y;
  pl->normal[2]=plane.Normal.Z;
  pl->dist = plane.D;

*/
  return (true);
  };

// Function
inline void RotateAroundX(float *point, float rcos, float rsin)
  {
  float y = point[1];
  float z = point[2];
  point[1] = (y * rcos) - (z * rsin);
  point[2] = (y * rsin) + (z * rcos);
  }

// Function
inline void RotateAroundY(float *point, float rcos, float rsin)
  {
  float x = point[0];
  float z = point[2];
  point[0] = (x * rcos) - (z * rsin);
  point[2] = (x * rsin) + (z * rcos);
  }

// Function
inline void RotateAroundZ(float *point, float rcos, float rsin)
  {
  float x = point[0];
  float y = point[1];
  point[0] = (x * rcos) - (y * rsin);
  point[1] = (x * rsin) + (y * rcos);
  }

// Function
inline void RotateAroundX(float *dest, float *point, float rcos, float rsin)
  {
  float y = point[1];
  float z = point[2];
  dest[0] = point[0];
  dest[1] = (y * rcos) - (z * rsin);
  dest[2] = (y * rsin) + (z * rcos);
  }

// Function
inline void RotateAroundY(float *dest, float *point, float rcos, float rsin)
  {
  float x = point[0];
  float z = point[2];
  dest[0] = (x * rcos) - (z * rsin);
  dest[1] = point[1];
  dest[2] = (x * rsin) + (z * rcos);
  }

// Function
inline void RotateAroundZ(float *dest, float *point, float rcos, float rsin)
  {
  float x = point[0];
  float y = point[1];
  dest[0] = (x * rcos) - (y * rsin);
  dest[1] = (x * rsin) + (y * rcos);
  dest[2] = point[2];
  }

char *stripwhitespace(char *line)
  {
  int   length = (int)strlen(line);
  bool  found = false;

  // Delete trailing newline
  if(line[length - 1] == '\n') line[length - 1] = '\0';
  for(int i = 0; i < length; i++, line++)
    {
    if(*line > 0x20)
      {
      found = true;
      break;
      }
    }

  if(found)
    {
    if(strncmp(line, "//", 2) == 0) return (NULL);
    return (line);
    }

  return (NULL);
  }

// Function
static void GetKeyPair(char *source, char *key, char *value)
  {
  char          *dest = key;
  unsigned int  pos = 0;

  // find key
  for(pos = 1; pos < strlen(source); pos++)
    {
    if(source[pos] != '\"')
      {
      *dest++ = source[pos];
      }
    else
      {
      *dest = 0;
      break;
      }
    }

  dest = value;
  pos++;                                      // skip the closing quote.

  // skip the white space and opening quote.
  for(; pos < strlen(source); pos++)
    {
    if(source[pos] == '\"')
      {
      pos++;
      break;
      }
    }

  for(; pos < strlen(source); pos++)
    {
    if(source[pos] != '\"')
      {
      *dest++ = source[pos];
      }
    else
      {
      *dest = 0;
      break;
      }
    }
  }

  stringlist                *mapstrings;              // only valid during load!
entgroup_6dx                *masterentgroup = NULL;
entgroup_6dx                *currentgroup;
entgroup_6dx                 toplevelgroup;
int facecount=0;



// Function
entgroup_6dx *AddEntity(entgroup_6dx *master, entgroup_6dx *group, char *classname, readent_t *re)
  {
  // Got an entity! Lets create it.
  entgroup_6dx  *result = NULL;

  /*
  if(stricmp(classname, "worldspawn") == 0)
    {
    for(int j = 0; j < re->keypaircount; j++)
      {
      keypair *kp = re->keypairs + j;
      if(stricmp(kp->keyword, "spawnflags") == 0) spawnflags = atoi(kp->value);
      if(stricmp(kp->keyword, "sun_vector") == 0)
        {
        // decode sun_vector into sun.
        sscanf(kp->value, "%f %f %f", &sun.x, &sun.y, &sun.z);
        usesun = true;
        }

      if(stricmp(kp->keyword, "sun_color") == 0)
        {
        // decode the sun color.
        sscanf(kp->value, "%f %f %f", &suncolor.x, &suncolor.y, &suncolor.z);
        usesun = true;
        }
      }

    result = NULL;
    }

  if(stricmp(classname, "info_player_start") == 0)
    {
    // Only one player start allowed.
    playerstart_loc[0] = re->origin[0];
    playerstart_loc[1] = re->origin[1];
    playerstart_loc[2] = re->origin[2];
    result = NULL;
    }

  if(stricmp(classname, "light") == 0)
    {
    // add a light to the master group.
    if(!master->Lights)
      {
      master->Lights = new TObjectList < light_6dx > ;
      }

    int       halo = 0;
    light_6dx *rlight = master->Lights->Append();
    rlight->active = 1;
    rlight->radius = 300;
    rlight->r = 1.0f;
    rlight->g = 1.0f;
    rlight->b = 1.0f;
    rlight->style = 0;
    rlight->sun = false;
    rlight->origin[0] = re->origin[0];
    rlight->origin[1] = re->origin[1];
    rlight->origin[2] = re->origin[2];
    for(int j = 0; j < re->keypaircount; j++)
      {
      keypair *kp = re->keypairs + j;
      if(strcmp(kp->keyword, "style") == 0) rlight->style = atoi(kp->value);
      if(strcmp(kp->keyword, "light") == 0) rlight->radius = (float)atof(kp->value);
      if(strcmp(kp->keyword, "_color") == 0)
        {
        sscanf(kp->value, "%f %f %f", &rlight->r, &rlight->g, &rlight->b);
        }

      if((strcmp(kp->keyword, "halo") == 0) || (strcmp(kp->keyword, "corona") == 0))
        {
        halo = atoi(kp->value);
        if(halo > 0)
          {
          billboard_poly  *bp = Coronas.Append();
          bp->textureno = 0;
          bp->height = halo;
          bp->width = halo;
          bp->origin[0] = re->origin[0];
          bp->origin[1] = re->origin[1];
          bp->origin[2] = re->origin[2];
          }
        }
      }

    for(j = 0; j < 3; j++)
      {
      rlight->mins[j] = rlight->origin[j] - rlight->radius;
      rlight->maxs[j] = rlight->origin[j] + rlight->radius;
      }

    if(rlight->style >= '0') rlight->style -= '0';
    if(rlight->style == 30)
      {
      rlight->style = 0;
      rlight->sun = true;
      }

    result = NULL;
    }

  if(stricmp(classname, "func_portal") == 0)
    {
    portalgroup_6dx *newgroup = new portalgroup_6dx(classname, re->origin);
    newgroup->classtype = clfunc_portalgroup;
    newgroup->portaldest = NULL;
    AddGroup(master, group, newgroup);
    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "spawnflags") == 0) newgroup->flags = atoi(kp->value);
      if((strcmp(kp->keyword, "target") == 0) || (strcmp(kp->keyword, "destination") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    newgroup->flags |= INVISIBLE;
    result = newgroup;
    }

  if(stricmp(classname, "func_train") == 0)
    {
    float     speed = 100.0f;
    float     wait = 0.0f;
    int       spawnflags = 0;

    // target
    // targetname
    qle_train *newgroup = new qle_train(classname, re->origin);
    AddGroup(master, group, newgroup);
    newgroup->classtype = clfunc_train;
    newgroup->flags = INVISIBLE | INSUBSTANTIAL | TRIGGER;
    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "sound") == 0) newgroup->triggersound = sounds.FindSoundObject(kp->value);
      if(strcmp(kp->keyword, "speed") == 0) speed = (float)atof(kp->value);
      if(strcmp(kp->keyword, "wait") == 0) wait = (float)atof(kp->value);
      if(strcmp(kp->keyword, "spawnflags") == 0) spawnflags = atoi(kp->value);
      if((strcmp(kp->keyword, "target") == 0) || (strcmp(kp->keyword, "destination") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    // add the mover.
    vec3_t  dirvec = { 0, 0, 0 };
    float   lip = 0;
    newgroup->pos_distance = 0.0f;
    newgroup->sticky = true;
    newgroup->wait = wait;
    SetupLateralMover(newgroup, dirvec, speed, lip, 0.0f, 0);
    newgroup->activate_action = t_activate_train;
    newgroup->action = t_ready;
    if(spawnflags & 2) newgroup->toggle = true;
    if(spawnflags & 1)
      {
      newgroup->action = t_activate_train;

      // start on
      }

    newgroup->spawnflags = spawnflags;
    result = newgroup;
    }

  if(stricmp(classname, "func_mover") == 0)
    {
    }

  if(stricmp(classname, "func_rotating") == 0)
    {
    float       speed = 10.0f;
    float       accel = 100.0f;
    float       maxspeed = 500.0f;
    int         spawnflags = 0;
    float       wait = 0;
    qle_rotater *newgroup = new qle_rotater(classname, re->origin);
    AddGroup(master, group, newgroup);
    newgroup->classtype = clfunc_rotater;
    newgroup->flags = INVISIBLE | INSUBSTANTIAL | TRIGGER;
    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "sound") == 0) newgroup->triggersound = sounds.FindSoundObject(kp->value);
      if(strcmp(kp->keyword, "speed") == 0) speed = (float)atof(kp->value);
      if(strcmp(kp->keyword, "wait") == 0) wait = (float)atof(kp->value);
      if(strcmp(kp->keyword, "spawnflags") == 0) spawnflags = atoi(kp->value);
      if(strcmp(kp->keyword, "minlight") == 0)
        {
        sscanf(kp->value, "%f %f %f", &newgroup->minlight[0], &newgroup->minlight[1], &newgroup->minlight[2]);
        }

      if((strcmp(kp->keyword, "target") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    vec3_t  dirvec = { 0, 0, 0 };
    float   lip = 0.0f;
    newgroup->pos_current = 0.0f;
    newgroup->sticky = false;
    newgroup->wait = wait;
    SetupLateralMover(newgroup, dirvec, speed, lip, 0.0f, 0);
    newgroup->toggle = true;
    newgroup->activate_action = t_rotating;
    newgroup->rotate_axis = 1;
    if(spawnflags & 4) newgroup->rotate_axis = 0;
    if(spawnflags & 8) newgroup->rotate_axis = 2;
    if(spawnflags & 1)
      {
      // start_on
      newgroup->action = t_rotating;
      }
    else
      {
      newgroup->action = t_ready;
      }

    newgroup->spawnflags = spawnflags;
    result = newgroup;
    }

  if(stricmp(classname, "func_door_rotating") == 0)
    {
    float       speed = 10.0f;
    float       accel = 100.0f;
    float       maxspeed = 500.0f;
    int         spawnflags = 0;
    float       wait = 0;
    qle_rotater *newgroup = new qle_rotater(classname, re->origin);
    AddGroup(master, group, newgroup);
    newgroup->classtype = clfunc_rotater;
    newgroup->flags = INVISIBLE | INSUBSTANTIAL | TRIGGER;
    newgroup->pos_distance = 90;
    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "sound") == 0) newgroup->triggersound = sounds.FindSoundObject(kp->value);
      if(strcmp(kp->keyword, "distance") == 0) newgroup->pos_distance = (float)atof(kp->value);
      if(strcmp(kp->keyword, "speed") == 0) speed = (float)atof(kp->value);
      if(strcmp(kp->keyword, "wait") == 0) wait = (float)atof(kp->value);
      if(strcmp(kp->keyword, "spawnflags") == 0) spawnflags = atoi(kp->value);
      if(strcmp(kp->keyword, "minlight") == 0)
        {
        sscanf(kp->value, "%f %f %f", &newgroup->minlight[0], &newgroup->minlight[1], &newgroup->minlight[2]);
        }

      if((strcmp(kp->keyword, "target") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    vec3_t  dirvec = { 0, 0, 0 };
    float   lip = 0.0f;
    newgroup->pos_current = 0.0f;
    newgroup->sticky = false;
    newgroup->wait = wait;
    SetupLateralMover(newgroup, dirvec, speed, lip, 0.0f, 0);
    newgroup->activate_action = t_opening;
    newgroup->rotate_axis = 1;
    newgroup->action = t_ready;
    if(spawnflags & 32) newgroup->toggle = true;
    if(spawnflags & 2) newgroup->reverse = true;  //reverse
    if(spawnflags & 64) newgroup->rotate_axis = 0;
    if(spawnflags & 128) newgroup->rotate_axis = 2;
    if(spawnflags & 1) newgroup->pos_current = newgroup->pos_distance;
    newgroup->spawnflags = spawnflags;
    result = newgroup;
    }

  if(stricmp(classname, "func_plat") == 0)        // or button, or plat.
    {
    //add a mover that acts like a door.
    // give it a 0 rotation amount.
    // and give it a length to move.
    float     speed = 100.0f;
    float     lip = 8.0f;
    float     wait = 3.0f;
    float     health = 0.0f;
    float     damage = 2.0f;
    float     height = 0.0f;
    int       spawnflags = 0;
    qle_mover *newgroup = new qle_mover(classname, re->origin);
    AddGroup(master, group, newgroup);
    newgroup->classtype = clfunc_mover;
    newgroup->flags = INVISIBLE | INSUBSTANTIAL | TRIGGER;
    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "sound") == 0) newgroup->triggersound = sounds.FindSoundObject(kp->value);
      if(strcmp(kp->keyword, "height") == 0) height = (float)atof(kp->value);
      if(strcmp(kp->keyword, "speed") == 0) speed = (float)atof(kp->value);
      if(strcmp(kp->keyword, "lip") == 0) lip = (float)atof(kp->value);
      if(strcmp(kp->keyword, "wait") == 0) wait = (float)atof(kp->value);
      if(strcmp(kp->keyword, "minlight") == 0)
        {
        sscanf(kp->value, "%f %f %f", &newgroup->minlight[0], &newgroup->minlight[1], &newgroup->minlight[2]);
        }

      if(strcmp(kp->keyword, "spawnflags") == 0) spawnflags = atoi(kp->value);
      if((strcmp(kp->keyword, "target") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    // add the mover.
    vec3_t  dirvec;
    dirvec[0] = 0;
    dirvec[1] = height / ((float)fabs(height));   // +1 or -1
    dirvec[2] = 0;
    newgroup->pos_distance = height;
    newgroup->pos_current = 0.0f;
    newgroup->sticky = true;
    newgroup->wait = wait;
    SetupLateralMover(newgroup, dirvec, speed, lip, 0.0f, 0);
    newgroup->toggle = true;
    newgroup->spawnflags = spawnflags;
    result = newgroup;
    }

  if(stricmp(classname, "func_door") == 0)        // or button, or plat.
    {
    //add a mover that acts like a door.
    // give it a 0 rotation amount.
    // and give it a length to move.
    int           angle = 0;
    int           team = -1;
    float         speed = 100.0f;
    float         lip = 8.0f;
    float         wait = 3.0f;
    float         health = 0.0f;
    float         damage = 2.0f;
    float         height = 0.0f;
    int           spawnflags = 0;


    qle_mover *newgroup = new qle_mover(classname, re->origin);
    AddGroup(master, group, newgroup);
    newgroup->classtype = clfunc_mover;
    newgroup->flags = INVISIBLE | INSUBSTANTIAL | TRIGGER;
    for(int i = 0; i < re->keypaircount; i++)
      {
      // add _minlight
      // add height
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "sound") == 0) newgroup->triggersound = sounds.FindSoundObject(kp->value);
      if(strcmp(kp->keyword, "angle") == 0) angle = atoi(kp->value);
      if(strcmp(kp->keyword, "speed") == 0) speed = (float)atof(kp->value);
      if(strcmp(kp->keyword, "lip") == 0) lip = (float)atof(kp->value);
      if(strcmp(kp->keyword, "wait") == 0) wait = (float)atof(kp->value);
      if(strcmp(kp->keyword, "spawnflags") == 0) spawnflags = atoi(kp->value);
      if(strcmp(kp->keyword, "minlight") == 0)
        {
        sscanf(kp->value, "%f %f %f", &newgroup->minlight[0], &newgroup->minlight[1], &newgroup->minlight[2]);
        }

      if((strcmp(kp->keyword, "target") == 0) || (strcmp(kp->keyword, "destination") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    // add the mover.
#ifndef NO_DOORS
    vec3_t  dirvec;
    if(angle >= 0)
      {
      float tangle = ((float)angle) / 180.0f;
      float radians = g_PI * tangle;
      dirvec[0] = (float)cos(radians);
      dirvec[1] = 0;
      dirvec[2] = (float)sin(radians);
      }
    else
      {
      // distance = (int)(mod->maxs[1]-mod->mins[1]);
      float verticaldir = -1.0f;
      if(angle == -1) verticaldir = 1.0f;
      dirvec[0] = 0;
      dirvec[1] = verticaldir;
      dirvec[2] = 0;
      }

    newgroup->pos_distance = 0.0f;
    newgroup->sticky = false;
    newgroup->angle = angle;
    newgroup->wait = wait;
    SetupLateralMover(newgroup, dirvec, speed, lip, 0.0f, 0);
    newgroup->action = t_setupdoor;
    if(spawnflags & 32) newgroup->toggle = true;
    if(spawnflags & 1) newgroup->startopen = true;
    newgroup->spawnflags = spawnflags;
#endif
    result = newgroup;
    }

  if(stricmp(classname, "func_button") == 0)
    {
    //add a mover that acts like a door.
    // give it a 0 rotation amount.
    // and give it a length to move.
    //add a mover that acts like a door.
    // give it a 0 rotation amount.
    // and give it a length to move.
    int       angle = 0;
    int       team = -1;
    float     speed = 40.0f;
    float     lip = 8.0f;
    float     wait = 1.0f;
    float     health = 0.0f;
    float     damage = 2.0f;
    float     height = 0.0f;
    qle_mover *newgroup = new qle_mover(classname, re->origin);
    AddGroup(master, group, newgroup);
    newgroup->classtype = clfunc_mover;
    newgroup->flags = INVISIBLE | INSUBSTANTIAL | TRIGGER;
    newgroup->spawnflags = 0;
    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "sound") == 0) newgroup->triggersound = sounds.FindSoundObject(kp->value);
      if(strcmp(kp->keyword, "angle") == 0) angle = atoi(kp->value);
      if(strcmp(kp->keyword, "speed") == 0) speed = (float)atof(kp->value);
      if(strcmp(kp->keyword, "lip") == 0) lip = (float)atof(kp->value);
      if(strcmp(kp->keyword, "wait") == 0) wait = (float)atof(kp->value);
      if(strcmp(kp->keyword, "spawnflags") == 0) newgroup->spawnflags = atoi(kp->value);
      if(strcmp(kp->keyword, "minlight") == 0)
        {
        sscanf(kp->value, "%f %f %f", &newgroup->minlight[0], &newgroup->minlight[1], &newgroup->minlight[2]);
        }

      if((strcmp(kp->keyword, "target") == 0) || (strcmp(kp->keyword, "destination") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    // add the mover.
    vec3_t  dirvec;
    if(angle >= 0)
      {
      float tangle = ((float)angle) / 180.0f;
      float radians = g_PI * tangle;
      dirvec[0] = (float)cos(radians);
      dirvec[1] = 0;
      dirvec[2] = (float)sin(radians);
      }
    else
      {
      // distance = (int)(mod->maxs[1]-mod->mins[1]);
      float verticaldir = -1.0f;
      if(angle == -1) verticaldir = 1.0f;
      dirvec[0] = 0;
      dirvec[1] = verticaldir;
      dirvec[2] = 0;
      }

    newgroup->pos_distance = 0.0f;
    newgroup->sticky = false;
    newgroup->angle = angle;
    newgroup->wait = wait;
    SetupLateralMover(newgroup, dirvec, speed, lip, 0.0f, 0);
    newgroup->action = t_setupdoor;
    if(newgroup->spawnflags & 32) newgroup->toggle = true;
    result = newgroup;
    }

  if(strnicmp(classname, "trigger_", 8) == 0)
    {
    qle_triggerbox  *newgroup = new qle_triggerbox(classname, re->origin);

    // entgroup_6dx *newgroup =
    AddGroup(master, group, newgroup);
    newgroup->classtype = cltrigger_box;
    newgroup->flags = INVISIBLE | INSUBSTANTIAL | TRIGGER | FORCEBRUSHES_INVISIBLE | FORCEBRUSHES_NOBLOCKING;
    if(strnicmp(classname, "trigger_once", 12) == 0)
      {
      newgroup->onlyonce = true;
      newgroup->never = false;
      newgroup->wait = FLT_MAX;
      }

    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "sound") == 0) newgroup->triggersound = sounds.FindSoundObject(kp->value);
      if(stricmp(kp->keyword, "wait") == 0) newgroup->wait = (float)atof(kp->value);
      if(strcmp(kp->keyword, "spawnflags") == 0) newgroup->spawnflags = atoi(kp->value);
      if((strcmp(kp->keyword, "target") == 0) || (strcmp(kp->keyword, "destination") == 0))
        {
        newgroup->target_id = slTargets.Add(kp->value);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    result = newgroup;
    }

  if(stricmp(classname, "func_group") == 0)
    {
    entgroup_6dx  *newgroup = new entgroup_6dx(classname, re->origin);
    AddGroup(master, group, newgroup);
    newgroup->classtype = clfunc_group;
    for(int i = 0; i < re->keypaircount; i++)
      {
      keypair *kp = re->keypairs + i;
      if(strcmp(kp->keyword, "spawnflags") == 0) newgroup->flags = atoi(kp->value);
      if(strcmp(kp->keyword, "minlight") == 0)
        {
        sscanf(kp->value, "%f %f %f", &newgroup->minlight[0], &newgroup->minlight[1], &newgroup->minlight[2]);
        }

      if(strcmp(kp->keyword, "targetname") == 0)
        {
        if(strcmp(kp->value, "outside") == 0) outsidegroup = toplevelgroup.Groups.count - 1;
        newgroup->targetname_id = slTargets.Add(kp->value);
        }
      }

    result = newgroup;
    }

  readent_t *new_re = Entities.Append();          // This entity is not used in the map. Maybe in the instance?
  new_re->keypairs = re->keypairs;
  new_re->keypaircount = re->keypaircount;
  new_re->group = group->groupno;
  VECTORCOPY(re->origin, new_re->origin);
  strcpy(new_re->classname, classname);
  re->keypairs = NULL;
  */
  return (result);
  }

  bool CreateFaces(readbrush_t *b)
  {
  /*
  readplane_t *p = b->rplanes;
  for(int i = 0; i < b->planecount; i++, p++)
    {
    // make a giant face for the side.
    PlaneFromPoints(p);
    CalcMasterFace(p);
    }

  p = b->rplanes;
  for(int i = 0; i < b->planecount; i++, p++)
    {
    // clip giant face versus brush.
    ClipFace(b, p);

    // get texture.
    p->mip = FindRmip(p->texture);
    }

  // Now check for valid faces.
  p = b->rplanes;
  for(int i = 0; i < b->planecount; i++, p++)
    {
    // collapse duplicate face points into one.
    if(!CollapsePoints(p)) return (false);
    }
*/
  return (true);
  };



vec3_t  baseaxis[18] =
  {
  { 0, 0, 1 },
  { 1, 0, 0 },
  { 0, -1, 0 },

  // floor
  { 0, 0, -1 },
  { 1, 0, 0 },
  { 0, -1, 0 },

  // ceiling
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, -1 },

  // west wall
  { -1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, -1 },

  // east wall
  { 0, 1, 0 },
  { 1, 0, 0 },
  { 0, 0, -1 },

  // south wall
  { 0, -1, 0 },
  { 1, 0, 0 },
  { 0, 0, -1 }                                // north wall
  };

// Function
void TextureAxisFromPlane(dplane_t *pln, core::vector3df xv, vec3_t yv)
  {
  /*
  int   bestaxis;
  float dot, best;
  int   i;
  best = 0;
  bestaxis = 0;
  for(i = 0; i < 6; i++)
    {
    dot = DotProduct(pln->normal, baseaxis[i * 3]);
    if(dot > best)
      {
      best = dot;
      bestaxis = i;
      }
    }

  VECTORCOPY(baseaxis[bestaxis * 3 + 1], xv);
  VECTORCOPY(baseaxis[bestaxis * 3 + 2], yv);
  */
  }

#define Q_PI  3.14159265358979323846

// Function
void GetUVCoords(face_6dx *f, readplane_t *rp)
  {
  /*
  float   u, v, nu, nv;
  float   ang, sinv, cosv;
  vec3_t  vecs[2];
  vec3_t  coord, p, p2, lu, lv;
  vec3_t  maxs, mins;
  f->lightmap = NULL;
  f->uv = NULL;
  if(f->properties & SURF_NODRAW) return;

  // get natural texture axis
  TextureAxisFromPlane(&f->plane, vecs[0], vecs[1]);
  ang = rp->rotation / 180.0f * g_PI;
  sinv = (float)sin(ang);
  cosv = (float)cos(ang);
  maxs[0] = -9999999.0f;
  maxs[1] = -9999999.0f;
  maxs[2] = -9999999.0f;
  mins[0] = 9999999.0f;
  mins[1] = 9999999.0f;
  mins[2] = 9999999.0f;

  vertex3 *vert = f->vertexes;
  vertex3 *vert2 = f->vertexes + 1;

  // Set up for lightmapping coordinates.
  p[0] = vert->x;
  p[1] = vert->y;
  p[2] = vert->z;
  p2[0] = vert2->x;
  p2[1] = vert2->y;
  p2[2] = vert2->z;

  // U is the u axis vector.
  VECTORSUBTRACT(p, p2, lu);
  VectorNormalize(lu);

  // V is the V axis vector.
  CrossProduct(f->plane.normal, lu, lv);

  uvcoord *uvc = new uvcoord[f->vertexcount];
  f->uv = uvc;
  for(int i = 0; i < f->vertexcount; i++, vert++)
    {
    vertex3 *vert = f->vertexes + i;
    coord[0] = vert->x;
    coord[1] = vert->y;
    coord[2] = vert->z;
    AddToMinMax(vert, mins, maxs);
    u = DotProduct(coord, vecs[0]);
    v = DotProduct(coord, vecs[1]);
    nu = cosv * u - sinv * v;
    nv = sinv * u + cosv * v;
    u = nu / rp->xscale + rp->xoff;
    v = nv / rp->yscale + rp->yoff;

    // scale
    vert->tu1 = u / f->mip->textures.items[0]->width;
    vert->tv1 = v / f->mip->textures.items[0]->height;
    }
    */
  }

  void ReverseVertexes(face_6dx *f)
  {
   /*
      vertex3 *vlist = new vertex3[f->vertexcount];
      for(int i = 0; i < f->vertexcount; i++)
        {
        vlist[i] = f->vertexes[f->vertexcount - 1 - i];
        }

      delete f->vertexes;
      f->vertexes = vlist;
      */
  }

  int loadentity(entgroup_6dx *group, unsigned int lineno, unsigned int grouplevel)
  {
  // first thing should be "classname", but who knows, eh?
  // read all the keywords/values.
  // read all the brushes.
  char        spacestring[256];
  char        classname[64];
  float       origin[3];
  stringlist  slBrushFaces;
  stringlist  keys;
  stringlist  values;
  spacestring[0] = 0;

  int brushcount = 0;
  for(unsigned int i = 0; i < grouplevel; i += 2)
    {
    spacestring[i] = 32;
    spacestring[i + 1] = 32;
    spacestring[i + 2] = 0;
    }

  classname[0] = 0;
//  VectorClear(origin);
  while(lineno < mapstrings->count)
    {
    char  *mapline = mapstrings->items[lineno++];
    if(*mapline == '\"')
      {
      // keypair - load it.
      char  key[1024];
      char  value[1024];
      GetKeyPair(mapline, key, value);
      keys.Append(key);
      values.Append(value);

      if(strcmp(key, "classname") == 0)
        {
        strcpy(classname, value);
        }

      if(strcmp(key, "origin") == 0)
        {
        sscanf(value, "%f %f %f", &origin[0], &origin[2], &origin[1]);
        }
      }

    if(*mapline == '{' || *mapline == '}')
      {
      // process the keypairs.
      // add the new group (if any) here.
      if(classname)
        {
        readent_t r_ent;
        r_ent.keypaircount = keys.count;
        r_ent.keypairs = new keypair[r_ent.keypaircount];
        for(unsigned int j = 0; j < keys.count; j++)
          {
          strcpy(r_ent.keypairs[j].keyword, SDL_strlwr(keys.items[j]));
          strcpy(r_ent.keypairs[j].value, SDL_strlwr(values.items[j]));
          }

//        VECTORCOPY(origin, r_ent.origin);

        entgroup_6dx  *newgroup = AddEntity(masterentgroup, group, classname, &r_ent);
        if(!r_ent.keypairs) delete[] r_ent.keypairs;
        if(newgroup)
          {
          group = newgroup;
          if(grouplevel == 0)
            {
            masterentgroup = group;
            }

          grouplevel++;
        //  Groups.Add(newgroup);
          }

        classname[0] = 0;
        keys.count = 0;
        values.count = 0;
        }
      }

      if(*mapline == '{')
      {
      lineno = loadentity(group, lineno, grouplevel);
      continue;
      }

      /*
      if(strnicmp(mapline, "patchdef2", 9) == 0)
      {
      patch_6dx *p = new patch_6dx;
      p->mat = &dispmatrix;

      LightMapVertex  *cp;
      char            *pline;
      pline = mapstrings->items[lineno++];    // the '{'
      pline = mapstrings->items[lineno++];    // the texture.
      p->mip = FindRmip(pline);
      if(!coalmip) coalmip = p->mip;

      char  *texture = pline;                 //     need to get the texture info for this. Point to mip directly?
      pline = mapstrings->items[lineno++];    // the patch info.
      sscanf(pline, "( %d %d %d %d %f)", &p->height, &p->width, &p->contents, &p->flags, &p->value);
      pline = mapstrings->items[lineno++];    // should be the open paren.

      // sanity check
      int count = p->width * p->height;
      if(count < 9 || count > 1024)
        {
        // die! this is bad!
        }

      p->cp = new LightMapVertex[count];
      cp = p->cp;

      // D3DVERTEX *v;
      for(int y = 0; y < p->height; y++)
        {
        pline = mapstrings->items[lineno++];  // should be the line to parse.
        pline += 2;                           // skip the first open paren
        for(int x = 0; x < p->width; x++, cp++)
          {
          // each vertex for this line.
          //read x,y,z,tu,tv from this line.
          // v = &p->Control[y][x];
          sscanf(pline, "( %f %f %f %f %f )", &cp->mPos.x, &cp->mPos.z, &cp->mPos.y, &cp->mTexel1.x, &cp->mTexel1.y); // &v->x,&v->z,&v->y,&v->tu,&v->tv);
          cp->mTexel2.x = (float)x / (float)p->width;   //0.0f; // should be lightmap coord.
          cp->mTexel2.y = (float)y / (float)p->height;  //fixme!!!

          //increment pline to the next open paren.
          int w = (int)strlen(pline);
          int pcount = 0;
          for(int i = 0; i < w; i++)
            {
            if(*pline == '(') pcount++;
            if(pcount == 2) break;                      // skip the current paren, and end up on the next open paren.
            pline++;
            }
          }
        }

      // p->Build(10);
      p->Build();
      group->Patches.Add(p);
      lineno++;                                         // skip the last close paren.
      lineno++;                                         // skip the last closing brace. // should we do this?
      lineno++;                                         // skip the last closing brace. // should we do this?
      return (lineno);


      }
      */

    if(*mapline == '(')
      {
      slBrushFaces.Append(mapline);
      }

    if(*mapline == '}')
      {
      // process the entity keys here...?
      if(slBrushFaces.count > 0)
        {
        readbrush_t brush;
        readplane_t *bp;
        brush.planecount = slBrushFaces.count;
        brush.rplanes = new readplane_t[slBrushFaces.count];
        for(unsigned int i = 0; i < slBrushFaces.count; i++)
          {
          bp = brush.rplanes + i;

          // get the data from the line.
          // ( 0 0 136 ) ( 0 10000 136 ) ( 10000 0 136 ) ship_plank1 0 0 0.0000 1.0000 1.0000 0 128 0
          bp->contents = 0;
          bp->surface = 0;
          bp->xoff = 0;
          bp->yoff = 0;
          bp->rotation = 0.0f;
          bp->xscale = 0.0f;
          bp->yscale = 0.0f;
          sscanf(slBrushFaces.items[i],
                 "  ( %f %f %f ) ( %f %f %f ) ( %f %f %f ) %s %d %d %f %f %f %d %d %f",
                 &bp->points[0][0],
                 &bp->points[0][1],
                 &bp->points[0][2],
                 &bp->points[1][0],
                 &bp->points[1][1],
                 &bp->points[1][2],
                 &bp->points[2][0],
                 &bp->points[2][1],
                 &bp->points[2][2],
                 &bp->texture[0],
                 &bp->xoff,
                 &bp->yoff,
                 &bp->rotation,
                 &bp->xscale,
                 &bp->yscale,
                 &bp->contents,
                 &bp->surface,
                 &bp->value);
          }

        brush_6dx *br = new brush_6dx;                  // group->Brushes.Append();
        if(!CreateFaces(&brush))
          {
          //int   SendEvent_Notify(char *message, int level);
          char  buffer[80];
          sprintf(buffer, "Brush %d has bad face. Brush discarded", br->brushnumber);
//          SendEvent_Notify(buffer, 1);
          delete br;                                    // create and delete so we know the brush number.
          continue;
          }

        br->contents = brush.rplanes[0].contents;


        // count the faces that point away.
        br->facecount = slBrushFaces.count;
        br->totalfaces = br->facecount;
        for(int i = 0; i < br->facecount; i++)
          {
          // if the face is not "nodraw", and it is marked "2 sided", then increment face count.
          readplane_t *rp = brush.rplanes + i;
          if(rp->surface & SURF_NODRAW)
            {
            // make sure there aren't any double-sided no draws.
            rp->surface &= (~SURF_DBLSIDE);
            continue;
            }

          if(rp->surface & SURF_DBLSIDE)
            {
            br->totalfaces++;
            }
          }

        br->faces = new face_6dx[br->totalfaces];

        // memset(br->faces,0,br->facecount*sizeof(face_6dx));
        br->mins[0] = 99999999.0f;
        br->mins[1] = 99999999.0f;
        br->mins[2] = 99999999.0f;
        br->maxs[0] = -99999999.0f;
        br->maxs[1] = -99999999.0f;
        br->maxs[2] = -99999999.0f;
        if(group->flags & FORCEBRUSHES_INVISIBLE)
          {
          br->visible = false;
          }

        if(group->flags & FORCEBRUSHES_NOBLOCKING)
          {
          br->blocking = false;
          }

        br->mastergroup = group->mastergroupno;

        int dblface = 0;
        for(int i = 0; i < br->facecount; i++)
          {
          face_6dx  *f = (face_6dx *)br->faces + i;

          // faces[facecount++] = f;
//          f->mat = &this->dispmatrix;

          readplane_t *rp = brush.rplanes + i;
          f->properties = rp->surface;
          if(group->classtype == clfunc_portalgroup)
            {
            if(f->properties & SURF_PORTAL)
              {
/*              portalgroup_6dx *gr = (portalgroup_6dx *)group;
              gr->portalface = f;
              gr->portaldest = NULL;*/
              }
            }

          f->minr = (int)(group->minlight.X * 255.0f);
          f->ming = (int)(group->minlight.Y * 255.0f);
          f->minb = (int)(group->minlight.Z * 255.0f);
         /* //if(spawnflags & WORLDFLAGS_ALLLIGHTMAPS) f->properties |= SURF_LIGHTMAP;
         // if(spawnflags & WORLDFLAGS_NOLIGHTMAPS) f->properties = f->properties & (!SURF_LIGHTMAP);
          if(rp->mip->additive)
            {
            f->properties |= SURF_COLORKEY;
            }

          br->surfaceproperties |= f->properties;
          if(br->contents & V2_CONTENTS_PLAYERCLIP) f->properties |= SURF_NODRAW;*/
          f->selected = false;
//          f->mip = rp->mip;
//          f->plane = rp->plane;
          f->vertexcount = rp->facepointcount;
          f->vertexes = new vertex3[rp->facepointcount];

          int color;
          if(f->properties & SURF_TRANS66)
            {
            color = 0x7fffffff;
            }
          else
            {
            color = 0xffffffff;
            }

          for(int k = 0; k < rp->facepointcount; k++)
            {
            // Must flip z/y later
            vertex3 *fv = f->vertexes + k;
            fv->x = (rp->facepoints[k][0]);
            fv->y = (rp->facepoints[k][1]);
            fv->z = (rp->facepoints[k][2]);
            fv->color = color;

            //fv->specular = 0x000c0c0c; TODO: did this to brighten the base texture. should probably keep it.
            }

          GetUVCoords(f, rp);
          for(int l = 0; l < f->vertexcount; l++)
            {
            vertex3 *v = f->vertexes + l;
            float   temp = v->z;
            v->z = v->y;
            v->y = temp;
            if(v->x > br->maxs[0]) br->maxs[0] = v->x;
            if(v->y > br->maxs[1]) br->maxs[1] = v->y;
            if(v->z > br->maxs[2]) br->maxs[2] = v->z;
            if(v->x < br->mins[0]) br->mins[0] = v->x;
            if(v->y < br->mins[1]) br->mins[1] = v->y;
            if(v->z < br->mins[2]) br->mins[2] = v->z;
            }

          ReverseVertexes(f);
          PlaneFromVertexes(f);
//          Faces.Add(f);
          facecount++;

           /* if(f->properties & SURF_DBLSIDE)
            {
            face_6dx  *destf = ((face_6dx *)br->faces) + br->facecount + dblface;
            dblface++;

            // duplicate face with reverse winding.
            *destf = *f;
            destf->brush = br;
            destf->vertexes = new vertex3[f->vertexcount];

            vertex3 *src = f->vertexes;
            vertex3 *dest = destf->vertexes;
            for(int i = 0; i < f->vertexcount; i++)
              {
              *dest = *src;
              src++;
              dest++;
              }

            bool  PlaneFromVertexes(face_6dx *f);
            ReverseVertexes(destf);

            // recalc plane
            PlaneFromVertexes(destf);
            }*/
          }

        /*  if(br->contents & V2_CONTENTS_ORIGIN)
          {
          if(group->classtype >= clfunc_mover)
            {
            qle_mover *qm = (qle_mover *)group;
            qm->found_origin = true;
            qm->rotation_origin[0] = (br->maxs[0] + br->mins[0]) / 2;
            qm->rotation_origin[1] = (br->maxs[1] + br->mins[1]) / 2;
            qm->rotation_origin[2] = (br->maxs[2] + br->mins[2]) / 2;
            }
          }
        else
          {
          // don't add an origin brush.
          br->parent = group;
          group->Brushes.Add(br);
          }*/

        delete[] brush.rplanes;
        }

      return (lineno);
      }
    }

  return (lineno);
  }



bool LoadMap(char *filename)
  {


  FILE  *f;
  char  line[1024];
  f = fopen(filename, "r");
  if(!f) return (false);
  char                                qlename[128];




  strcpy(qlename, filename);
  mapstrings = new stringlist;
  while(1)
    {
    char  *start;
    if(fgets(line, 1024, f) == NULL) break;
    start = stripwhitespace(line);
    if(!start) continue;
    mapstrings->Append(start);
    }

  fclose(f);

  unsigned int  lineno = 0;
  masterentgroup = &toplevelgroup;
  while(lineno < mapstrings->count)
    {
    char  *mapline = mapstrings->items[lineno++];
    if(*mapline == '{')
      {
      // add a group for this entity.      then load it.
      lineno = loadentity(&toplevelgroup, lineno, 0);
      }
    }

  delete mapstrings;
  /*
  CalcMinMaxs_r(&toplevelgroup);
  VECTORCOPY(toplevelgroup.mins, lmins);
  VECTORCOPY(toplevelgroup.maxs, lmaxs);
  FixupPortals();
  for(unsigned int i = 0; i < Groups.count; i++)
    {
    entgroup_6dx  *gr = Groups.items[i];
    if(gr->classtype == clfunc_train)
      {
      qle_train *grt = (qle_train *)gr;
      FixupTrains_r(gr, grt->rotation_origin);
      }
    }

  info.facecount = facecount;
  CreateLightmaps();
  for(i = 0; i < Faces.count; i++)
    {
    face_6dx  *f = Faces.items[i];
    if(f->faceflags & FACEFLAGS_UPDATELIGHT)
      {
      LitFaces.Add(f);
      }
    }

  for(unsigned int i = 0; i < qle_mips.count; i++)
    {
    rmip_t  *mip = qle_mips.items[i];
    if(!mip->ptexTexture)
      {
      bool  TextureFromMip(rmip_t *rmip);
      TextureFromMip(mip);
      }
    }
*/
  return (true);
  }




core::matrix4 createTransformation(core::vector3df RelativeTranslation,
core::vector3df RelativeScale =core::vector3df(1.f,1.f,1.f),
core::vector3df RelativeRotation=core::vector3df(0,0,0))
{
    core::matrix4 mat;
    mat.setRotationDegrees(RelativeRotation);
    mat.setTranslation(RelativeTranslation);

    if (RelativeScale != core::vector3df(1.f,1.f,1.f))
    {
        core::matrix4 smat;
        smat.setScale(RelativeScale);
        mat *= smat;
    }

    return mat;
}

int main(int argc, char *args[]) {




engine=irr::scene::Engine::createEngine(DISP_WIDTH,DISP_HEIGHT,false,TITLE);




window_size_callback(DISP_WIDTH,DISP_HEIGHT);





//size_t LengthOfArray = sizeof myArray / sizeof myArray[0];










scene::IGameScene* mainScene=engine->createScene("main",0);
scene::IGameScene* detailScene=engine->createScene("detail",1);

/*
        core::array<core::stringc> faces;
        faces.push_back("media/skybox/right.jpg");
        faces.push_back("media/skybox/left.jpg");
        faces.push_back("media/skybox/top.jpg");
        faces.push_back("media/skybox/bottom.jpg");
        faces.push_back("media/skybox/front.jpg");
        faces.push_back("media/skybox/back.jpg");

scene::ISkyBox* skybox= new ISkyBox(faces);

mainScene->addGameStatic(skybox);


*/
/*

scene::IMesh* cube_mesh= engine->getGeometryCreator()->createCubeMesh();
cube_mesh->getMeshBuffer(0)->getMaterial().setTexture(0,engine->getTexture("media/marble.jpg"));
mainScene->createGameObject("cube")->addComponent<IMeshRenderer>(cube_mesh);
*/



/*
scene::IAnimatedMesh* nijaMesh= engine->getGeometryCreator()->importMesh("media/ninja.ms3d");
scene::IGameObject* ninja=mainScene->createGameObject("ninja",3);
ninja->addComponent<IMeshRenderer>(nijaMesh);
ninja->addComponent<IAnimator>(nijaMesh);
ninja->Transform->setPosition(20,0,-8);

*/

scene::IAnimatedMesh* meshModel= engine->getGeometryCreator()->importMesh("media/room.3ds");
engine->getMeshManipulator()->makePlanarTextureMapping(meshModel->getMesh(0), 0.004f);
engine->getMeshManipulator()->scale(meshModel->getMesh(0), core::vector3df(0.1f,0.1f,0.1f));
meshModel->getMeshBuffer(0)->getMaterial().setTexture(0,engine->getTexture("media/wall.jpg"));
meshModel->setDirty();
scene::IGameObject* room=mainScene->createGameObject("room",2);
room->addComponent<IMeshRenderer>(meshModel);




/*

scene::IAnimatedMesh* md2Mesh= engine->getGeometryCreator()->importMesh("media/faerie.md2");
md2Mesh->getMeshBuffer(0)->getMaterial().setTexture(0,engine->getTexture("media/faerie2.bmp"));
//md2Mesh->getMeshBuffer(0)->getMaterial().setTexture(0,engine->getTexture("media/terrain-heightmap.bmp"));
scene::IGameObject* faerie=mainScene->createGameObject("faerie",3);
faerie->addComponent<IMeshRenderer>(md2Mesh);
faerie->addComponent<IAnimator>(md2Mesh);
*/
/*
scene::IGameObject* terrain=detailScene->createGameObject("terrain",4);
terrain->addComponent<IGeoTerrain>(core::vector3df(40.f, 4.4f, 40.f),5,scene::ETPS_33,4);
std::shared_ptr<IGeoTerrain>terrain_data=terrain->getComponent<IGeoTerrain>();
terrain_data.get()->loadHeightMap("media/terrain-heightmap.bmp");
terrain_data.get()->getMaterial().setTexture(0,engine->getTexture("media/terrain-texture.jpg"));
terrain_data.get()->getMaterial().setTexture(1,engine->getTexture("media/detailmap3.jpg"));
*/
/*

scene::IMeshAnimator* md2animator = new CMeshAnimator(quakemd2);
md2animator->grab();
md2animator->setMD2Animation(scene::EMAT_POINT);
md2animator->setAnimationSpeed(20.f);
scene::IShadowMesh* md2Shadow= new scene::CShadowMesh(md2animator->getMesh());
*/

//ninja->Transform->setPosition(2700*2,255*2,2600*2);
//ninja->Transform->setPosition(200,400,200);


int frame=0;

bool quit = false;
while (!quit && !engine->ShouldClose())
{

       SDL_Event event;
       while (SDL_PollEvent(&event))
        {


          if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE )
                quit = true;

         switch (event.type)
            {
            case SDL_QUIT:
            {
             quit = true;
             break;
            }
            case SDL_WINDOWEVENT:
            {

                 switch (event.window.event)
                 {
                    case SDL_WINDOWEVENT_CLOSE:
                     {
                     break;
                     }
                     case SDL_WINDOWEVENT_RESIZED:
                     {

                     window_size_callback(event.window.data1,event.window.data2);

                     break;
                     }
                      case SDL_WINDOWEVENT_SIZE_CHANGED:
                     {
                     break;
                     }


                  }
            break;
            }
             case SDL_MOUSEBUTTONDOWN:
                    {

                     engine->MouseButtonCallback(event.button.button-1,1,0);
                     engine->MouseCursorPosCallback((double)event.button.x,(double)event.button.y);
                    isMouseDown=false;

                    }
                    break;
                    case SDL_MOUSEBUTTONUP:
                    {
                     engine->MouseButtonCallback(event.button.button-1,0,0);
                     engine->MouseCursorPosCallback(event.button.x,event.button.y);
                     isMouseDown=false;

                    }
                    break;
                    case SDL_MOUSEMOTION:
                    {

                     engine->MouseCursorPosCallback(event.button.x,event.button.y);
                     break;
                    }

            case SDL_MOUSEWHEEL:
                {
                        engine->MouseScrollCallback(event.wheel.x,event.wheel.y);

                       if(event.wheel.y > 0) // scroll up
                        {
                           //  MouseWheel-=1;
                           fov -= 1;
                        }
                        else if(event.wheel.y < 0) // scroll down
                        {
                         fov += 1;
                        }


                    if (fov < 1.0f)
                    fov = 1.0f;
                    if (fov > 60.0f)
                    fov = 60.0f;



                    //pMat = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

                        core::matrix4 pMat;
                        pMat.buildProjectionMatrixPerspectiveFovRH(core::degToRad(fov),aspect, 0.1f, 42000.0f);
                        engine->setCameraFov(core::degToRad(fov));
                        engine->setCameraAspect(aspect);
                        engine->setProjectionMatrix(pMat);

                  break;
                }

                  case SDL_KEYDOWN:
                       {
                            engine->KeyCallback(event.key.keysym.scancode,event.key.keysym.sym,1,event.key.keysym.mod);
                       }
                       break;
                       case SDL_KEYUP:
                       {
                       engine->KeyCallback(event.key.keysym.scancode,event.key.keysym.sym,0,event.key.keysym.mod);
                       }
                      break;

     }
    }

    //engine->PollInputEvents();

    mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
    quit=true;
    }
    processInput();
    mouse_callback((double)mouse_x,(double)mouse_y);



    if (engine->IsKeyPressed(SDL_SCANCODE_O))
    {
      SDL_Log("o press");
    }

    if (engine->IsKeyDown(SDL_SCANCODE_O))
    {
      SDL_Log("o down");
    }

    if (engine->IsKeyReleased(SDL_SCANCODE_O))
    {
      SDL_Log("o release");
    }



/*
    if (engine->IsKeyUp(SDL_SCANCODE_O))
    {
      SDL_Log("o up");
    }
*/

    engine->setViewMatrix(cameraPos, cameraPos + cameraFront, cameraUp);

     detailScene->OnUpdate(os::Timer::getTime());
     mainScene->OnUpdate(os::Timer::getTime());





    engine->BeginScene();

    engine->ClearScene(video::ECBF_COLOR | video::ECBF_DEPTH,RAYWHITE );






IShader* shader = engine->getShader("LightDetailShader");
detailScene->OnRender(shader);

shader = engine->getShader("LightShader");
mainScene->OnRender(shader);



//mainScene->OnRenderStatic();


BeginModeBatch3D();

Draw3DGrid(20,1);
EndRenderBatch();

//RenderBatch();

//engine->setDepthTest(false);

//engine->render3DBatch();

//engine->setDepthTest(true);





engine->EndScene();


engine->Swap();


     // SDL_SetWindowTitle(window,TextFormat("fps[ %d ] %f %f ",GetFPS(),GetFrameTime(),GetTime()));
   engine->setWindowCaption(TextFormat("%s   fps[ %d ] %d %d %d %d",TITLE,engine->GetFPS(),engine->GetKeyPressed(),engine->GetCharPressed(),engine->GetTouchX(),engine->GetTouchY()));


}




engine->drop();


return EXIT_SUCCESS;
}
