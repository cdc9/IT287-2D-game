#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "graphics.h"
#include "simple_logger.h"
#include "sprite.h"
#include "player.h"
#include "entity.h"
#include "camera.h"

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect _Camera;
extern int LEVEL_WIDTH = 1024;
extern int LEVEL_HEIGHT = 768;

void Init_All();
void Init_Player();
void Init_Dummy();

int getImagePathFromFile(char *filepath,char * filename);
int getCoordinatesFromFile(int *x, int *y,char * filename);
void addCoordinateToFile(char *filepath,int x, int y);


Vec2d camSize;
/**
 * @brief Main game loop. Initialize everything and have a loop for update functions.
 */
int main(int argc, char *argv[])
{
  SDL_Surface *temp = NULL;
  //New variables
  int done;
  int mx,my;

  int doOnce = 0;
  int tx = 0,ty = 0;
  const Uint8 *keys;
  char imagepath[512];
  Sprite *sprite;
  Sprite *bg;
  SDL_Rect srcRect={0,0,800,600};
  Init_All();
  temp = IMG_Load("images/bgtest2.png");/*notice that the path is part of the filename*/
  if(temp != NULL)
  {
      printf("temp image loaded successfully\n");
      SDL_BlitSurface(temp,NULL,buffer,NULL);
  }
  slog("got here");
  gt_graphics_render_surface_to_screen(temp,srcRect,0,0); //Perhaps putting camera here will work?
  SDL_FreeSurface(temp);
  slog("got here");
  done = 0;
  bg = loadSprite("images/bgtest2.png",1024,768,1);

  do
  {
	SDL_RenderClear(gt_graphics_get_active_renderer());
	//drawSprite(bg,0,vec2d(0,0)); 
	drawSprite(bg,0,vec2d(0-_Camera.x,0-_Camera.y));
	update(); 
    NextFrame();
    SDL_PumpEvents();
    keys = SDL_GetKeyboardState(NULL);
/*
	if(doOnce == 0)
	{
		SpawnPlayer(20,20);
		doOnce = 1;
	}
	*/
    if(keys[SDL_SCANCODE_ESCAPE])
    {
        done = 1;
    }
  }while(!done);
  slog("got here");
  exit(0);		/*technically this will end the program, but the compiler likes all functions that can return a value TO return a value*/
  return 0;
}

void Init_All()
{
	float bgcolor[] = {1,1,1,1};
  Init_Graphics(
	"Game Test",
    800,
    600,
    800,
    600,
    bgcolor,
    0);
	initEntitySystem(255);
	initSpriteSystem(255); //TODO
	Init_Player();
	Init_Dummy();
	camSize.x = 168;
	camSize.y = 120;
	camera_set_size(camSize);
	//DefaultConfig();

}

void Init_Player()
{
	SpawnPlayer(350,200);
}
void Init_Dummy()
{
	SpawnDummy(500,200);
}
int getImagePathFromFile(char *filepath,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int returnValue = -1;
    if (!filepath)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    if (fscanf(fileptr,"%s",buf))
    {
        if (strcmp(buf,"image:")==0)
        {
            fscanf(fileptr,"%s",filepath);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    return returnValue;
}

void addCoordinateToFile(char *filepath,int x, int y)
{
    FILE *fileptr = NULL;
    if (!filepath)
    {
        fprintf(stdout,"addCoordinateToFile: warning, no input file path provided\n");
        return;
    }
    fileptr = fopen(filepath,"a");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filepath);
        return;
    }
    fprintf(fileptr,"%s:%i:newcoordinate: %i %i\n",__FILE__,__LINE__,x,y);
    fclose(fileptr);
}

int getCoordinatesFromFile(int *x, int *y,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int tx,ty;
    int returnValue = -1;
    if ((!x)&&(!y))
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    while (fscanf(fileptr,"%s",buf) != EOF)
    {
        fprintf(stdout,"buf is: %s\n",buf);
        if (strcmp(buf,"position:")==0)
        {
            fscanf(fileptr,"%i %i",&tx,&ty);
            fprintf(stdout,"as read: %i, %i\n",tx,ty);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    if (x)*x = tx;
    if (y)*y = ty;
    return returnValue;
}
