//File for project 
//3350 , spring 2022
//Gisela Jimenez Rojas


//souce for images:

//https://www.artstation.com/artwork/k413Ly
//https://www.seekpng.com/ipng/u2q8e6r5u2u2t4t4_preview-snow-pixel-art-tileset/

//next step put movement 

#include <iostream>
#include <fstream>
using namespace std; 
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
//#include "image.h"
//#include "credits.h"
#include "gjimenezroja.h"
//#include "danzaldo.h"


/*void gjimenezroja(int inventory){
   if(inventory < 101) {
      printf("You have this many apples %i", inventory);
   } else if (inventory < 0){
      printf("You have no apples"); 
   } else {
      printf("Max capacity reached"); 
   }
}*/

class Image {
public:
    int width, height, max;
    char *data;
    Image() { }
    Image(const char *fname) {
        bool isPPM = true;
        char str[1200];
        char newfile[200];
        ifstream fin;
        char *p = strstr((char *)fname, ".ppm");
        if (!p) {
            //not a ppm file
            isPPM = false;
            strcpy(newfile, fname);
            newfile[strlen(newfile)+4] = '\0';
            strcat(newfile, ".ppm");
            sprintf(str, "convert %s %s", fname, newfile);
            system(str);
            fin.open(newfile);
        } else {
            fin.open(fname);
        }
        char p6[10];
        fin >> p6;
        fin >> width >> height;
        fin >> max;
        data = new char [width * height * 3];
        fin.read(data, width * height * 3);
        fin.close();
        if (!isPPM)
            unlink(newfile);
    }
}  starts("images/start_screen.png"),
   enemy("images/icemonster.png"),
   snow("images/snow_map.png"),
   girl("images/char.png");
  
struct Vector {
    float x,y,z;
};

enum {
    STATE_INTRO,
    STATE_LEVEL_THREE,
    //STATE_GAME_OVER,
};

typedef double Flt;
//a game object
class Sprite {
public:
    Flt pos[3];      //vector
    Flt vel[3];      //vector
    float w, h;
    unsigned int color;
    bool alive_or_dead;     
    void set_dimensions(int x, int y) {
        w = (float)x * 0.05;
        h = w;
    }
    Sprite() {
        w = h = 4.0;
        pos[0] = 1.0;
        pos[1] = 200.0;
        vel[0] = 4.0;
        vel[1] = 0.0;
    }
};

class Global {
public:
    int xres, yres;
    Sprite sprite_three[2];
    Sprite sprite_lvl3[2]; 
    unsigned int texid_start;
    unsigned int texid_three;
    unsigned int spriteid_three;
    unsigned int spriteid_lvl3; 
    //the box components
    float pos[2];
    float w;
    float dir;
    int inside;
    Flt gravity;
    int frameno;
    int state;
    Global() {
        xres = 400;
        yres = 200;
        //box
        w = 20.0f;
        pos[0] = 0.0f + w;
        pos[1] = yres/2.0f;
        dir = 25.0f;
        inside = 0;
        gravity = 20.0;
        frameno = 1.0;
        state = STATE_INTRO;
    }
} s;

void init_start_screens() {
    //OpenGL initialization
    glViewport(0, 0, s.xres, s.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, s.xres, 0, s.yres, -1, 1);
    //
    //glDisable(GL_LIGHTING);
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_FOG);
    //glDisable(GL_CULL_FACE);
    //
    //Clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();

    //background 
    glGenTextures(1, &s.texid_start);
    glBindTexture(GL_TEXTURE_2D, s.texid_start);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, starts.width, starts.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, starts.data);
}

void level_select_screens() {
    Rect r;

    unsigned int c = 0x00ffff44;
    r.bot = 65;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, c, "1 - danzaldo level");
    ggprint8b(&r, 16, c, "2 - mlara2 level");
    ggprint8b(&r, 16, c, "3 - gjimenezroja level");
    ggprint8b(&r, 16, c, "4 - msteiner level");
    ggprint8b(&r, 16, c, "To select level type the corresponding number");
}

void render_start_screens() {
    if (s.state == STATE_INTRO) {
        glClear(GL_COLOR_BUFFER_BIT);     
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, s.texid_start);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      s.yres);
            glTexCoord2f(1,0); glVertex2i(s.xres, s.yres);
            glTexCoord2f(1,1); glVertex2i(s.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        level_select_screens();
    }
}

void init_level_three() {
    //OpenGL initialization
    glViewport(0, 0, s.xres, s.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, s.xres, 0, s.yres, -1, 1);
    //
    //glDisable(GL_LIGHTING);
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_FOG);
    //glDisable(GL_CULL_FACE);
    //
    //Clear the screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT);
    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();

    //background snow
    glGenTextures(1, &s.texid_three);
    glBindTexture(GL_TEXTURE_2D, s.texid_three);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, snow.width, snow.height, 0,
                               GL_RGB, GL_UNSIGNED_BYTE, snow.data);
   
   unsigned char *data1 = new unsigned char 
        [enemy.width * enemy.height * 4];
    for (int i=0; i<enemy.height; i++) {
        for (int j=0; j<enemy.width; j++) {
            int offset  = i*enemy.width*3 + j*3;
            int offset2 = i*enemy.width*4 + j*4;
            data1[offset2+0] = enemy.data[offset+0];
            data1[offset2+1] = enemy.data[offset+1];
            data1[offset2+2] = enemy.data[offset+2];
            data1[offset2+3] =
            ((unsigned char)enemy.data[offset+0] != 0 &&
             (unsigned char)enemy.data[offset+1] != 0 &&
             (unsigned char)enemy.data[offset+2] != 0);
}
    }
    //sprite enemy
    glGenTextures(1, &s.spriteid_three);
    glBindTexture(GL_TEXTURE_2D, s.spriteid_three);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, enemy.width, enemy.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data1);
    delete [] data1;
    s.sprite_three[0].set_dimensions(s.xres, s.yres);

    unsigned char *data2 = new unsigned char 
        [girl.width * girl.height * 4];
    for (int i=0; i<girl.height; i++) {
        for (int j=0; j<girl.width; j++) {
            int offset  = i*girl.width*3 + j*3;
            int offset2 = i*girl.width*4 + j*4;
            data2[offset2+0] = girl.data[offset+0];
            data2[offset2+1] = girl.data[offset+1];
            data2[offset2+2] = girl.data[offset+2];
            data2[offset2+3] =
            ((unsigned char)girl.data[offset+0] != 255 &&
             (unsigned char)girl.data[offset+1] != 255 &&
             (unsigned char)girl.data[offset+2] != 255);
        }
    }
    //sprite char
    glGenTextures(1, &s.spriteid_lvl3);
    glBindTexture(GL_TEXTURE_2D, s.spriteid_lvl3);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, girl.width, girl.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data2);
    delete [] data2;
    s.sprite_lvl3[0].set_dimensions(s.xres, s.yres);
}


void select_level_three() {
    s.state = STATE_LEVEL_THREE;
}
void select_start_screens(){
    s.state = STATE_INTRO;
}

void ssprite_move_right() {
    s.sprite_three[0].pos[0] += 10;
}

void ssprite_move_left() {
    s.sprite_three[0].pos[0] -= 10;
}

void ssprite_move_up() {
    s.sprite_three[0].pos[1] += 10;
}

void ssprite_move_down() {
    s.sprite_three[0].pos[1] -= 10;
}

void physics_level_three() {
    ++s.frameno;
    if (s.frameno > 20)
        s.frameno = 1;
    //movement
    s.sprite_three[0].pos[0] += s.sprite_three[0].vel[0];
    s.sprite_three[0].pos[1] += s.sprite_three[0].vel[1];
    //boundry test
    if (s.sprite_three[0].pos[0] >= s.xres) {
        s.sprite_three[0].pos[0] = s.xres;
        s.sprite_three[0].vel[0] = 0.0;
    }
    if (s.sprite_three[0].pos[0] <= 0) {
        s.sprite_three[0].pos[0] = 0;
        s.sprite_three[0].vel[0] = 0.0;
    }
    if (s.sprite_three[0].pos[1] >= s.yres) {
        s.sprite_three[0].pos[1] = s.yres;
        s.sprite_three[0].vel[1] = 0.0;
    }
    if (s.sprite_three[0].pos[1] <= 0) {
        s.sprite_three[0].pos[1] = 0;
        s.sprite_three[0].vel[1] = 0.0;
    }
    
    Flt cx = s.xres/2.0;
    Flt cy = s.yres/2.0;
    cx = s.xres * (218.0/300.0);
    cy = s.yres * (86.0/169.0);
    Flt dx = cx - s.sprite_three[0].pos[0];
    Flt dy = cy - s.sprite_three[0].pos[1];
    Flt dist = (dx*dx + dy*dy);
    if (dist < 0.01)
        dist = 0.01; 
    s.sprite_three[0].vel[0] += (dx / dist) * s.gravity;
    s.sprite_three[0].vel[1] += (dy / dist) * s.gravity;
    s.sprite_three[0].vel[0] += ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
    s.sprite_three[0].vel[1] += ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
}
void render_level_three() {
    if (s.state == STATE_LEVEL_THREE) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, s.texid_three);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      s.yres);
            glTexCoord2f(1,0); glVertex2i(s.xres, s.yres);
            glTexCoord2f(1,1); glVertex2i(s.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

      //Draw girl sprite
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glTranslatef(s.sprite_three[0].pos[0], s.sprite_three[0].pos[1], 0.0f);
        //set alpha test
        //https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/
        //xhtml/glAlphaFunc.xml
        glEnable(GL_ALPHA_TEST);
        //transparent if alpha value is greater than 0.0
        glAlphaFunc(GL_GREATER, 0.0f);
        //Set 4-channels of color intensity
        glColor4ub(255,255,255,255);
        //
        glBindTexture(GL_TEXTURE_2D, s.spriteid_three);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-s.sprite_three[0].w, 
                                          -s.sprite_three[0].h);
            glTexCoord2f(0, 0); glVertex2f(-s.sprite_three[0].w,  
                                           s.sprite_three[0].h);
            glTexCoord2f(1, 0); glVertex2f( s.sprite_three[0].w,  
                                           s.sprite_three[0].h);
            glTexCoord2f(1, 1); glVertex2f( s.sprite_three[0].w, 
                                          -s.sprite_three[0].h);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();

        Rect r;
        unsigned int c = 0x00ffff44;
        r.bot = s.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, c, "gjimenezroja's Level");
        r.bot = 20;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, "To select level type the corresponding number");
    }
}

//render_start_screen(); 

unsigned char *buildAlphaDataS(Image *img)
{
	//add 4th component to RGB stream...
	int i;
	unsigned char *newdata, *ptr;
	unsigned char *data = (unsigned char *)img->data;
	newdata = (unsigned char *)malloc(img->width * img->height * 4);
	ptr = newdata;
	unsigned char a,b,c;
	//use the first pixel in the image as the transparent color.
	unsigned char t0 = *(data+0);
	unsigned char t1 = *(data+1);
	unsigned char t2 = *(data+2);
	for (i=0; i<img->width * img->height * 3; i+=3) {
		a = *(data+0);
		b = *(data+1);
		c = *(data+2);
		*(ptr+0) = a;
		*(ptr+1) = b;
		*(ptr+2) = c;
		*(ptr+3) = 1;
		if (a==t0 && b==t1 && c==t2)
			*(ptr+3) = 0;
		//-----------------------------------------------
		ptr += 4;
		data += 3;
	}
	return newdata;
}
