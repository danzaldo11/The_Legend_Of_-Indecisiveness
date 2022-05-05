// Author: Mariana Lara
// Project Software Eng.
//
// 

//Websites used:
// Girl Character: https://sanderfrenken.github.io/Universal-LPC-Spritesheet-Character-Generator/#?body=Humanlike_white&sex=female&shadow=none&nose=Straight_nose_tanned&eyes=Eyes_brown&hair=Curly_long_light_brown&hat=none&visor=none&earring=Earring_gold&necklace=Necklace_gold&shoulders=none&arms=none&dress=none&clothes=Tanktop_yellow&legs=Pants_teal&shoes=Sandals_brown&quiver=none&weapon=Thrust_shovel
// Beach Map:      https://www.deviantart.com/erichkisaragi/art/Beach-House-604911358
// Crab:           https://www.pngegg.com/en/png-klfdd


#include <stdio.h>
#include <string.h>
#include <cmath>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std; 
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "mlara2.h"
#include "danzaldo.h"


//checks armour level
 void mlara2_armour(int a){
   
      if(a <= 0){
          printf("Armour level low, pause to level up.\n");
          a++;
 
      } else {
          printf("Armour: %i\n", a);
          a--;
      }
}

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
}  start2("images/start_screen.png"),
   crab("images/crab.png"),
   beach("images/beach_map.png"),
   girl2("images/beach_girl.png");

struct Vector {
    float x,y,z;
};

enum {
    STATE_INTRO,
    STATE_LEVEL_TWO,
};

typedef double Flt;

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
    Sprite sprite_two[2];
    Sprite sprite_lvl2[2]; 
    unsigned int texid_start;
    unsigned int texid_two;
    unsigned int spriteid_two;
    unsigned int spriteid_lvl2; 
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
} m;

void init_start_screen2() {
    //OpenGL initialization
    glViewport(0, 0, m.xres, m.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, m.xres, 0, m.yres, -1, 1);
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
    glGenTextures(1, &m.texid_start);
    glBindTexture(GL_TEXTURE_2D, m.texid_start);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, start2.width, start2.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, start2.data);
}

void level_select_screen2() {
    Rect r;

    unsigned int c = 0x00ffffff;
    r.bot = 65;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, c, "1 - Level 1");
    ggprint8b(&r, 16, c, "2 - Level 2");
    ggprint8b(&r, 16, c, "3 - Level 3");
    ggprint8b(&r, 16, c, "4 - Level 4");
    ggprint8b(&r, 16, c, "To select level type the corresponding number");
}

void render_start_screen2() {
    if (m.state == STATE_INTRO) {
        glClear(GL_COLOR_BUFFER_BIT);     
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, m.texid_start);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      m.yres);
            glTexCoord2f(1,0); glVertex2i(m.xres, m.yres);
            glTexCoord2f(1,1); glVertex2i(m.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        //level_select_screens();
    }
}

void init_level_two() {
    //OpenGL initialization
    glViewport(0, 0, m.xres, m.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, m.xres, 0, m.yres, -1, 1);
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
    glGenTextures(1, &m.texid_two);
    glBindTexture(GL_TEXTURE_2D, m.texid_two);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, beach.width, beach.height, 0,
                               GL_RGB, GL_UNSIGNED_BYTE, beach.data);
   
   unsigned char *data1 = new unsigned char 
        [crab.width * crab.height * 4];
    for (int i=0; i<crab.height; i++) {
        for (int j=0; j<crab.width; j++) {
            int offset  = i*crab.width*3 + j*3;
            int offset2 = i*crab.width*4 + j*4;
            data1[offset2+0] = crab.data[offset+0];
            data1[offset2+1] = crab.data[offset+1];
            data1[offset2+2] = crab.data[offset+2];
            data1[offset2+3] =
            ((unsigned char)crab.data[offset+0] != 0 &&
             (unsigned char)crab.data[offset+1] != 0 &&
             (unsigned char)crab.data[offset+2] != 0);
}
    }
    //sprite enemy
    glGenTextures(1, &m.spriteid_two);
    glBindTexture(GL_TEXTURE_2D, m.spriteid_two);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, crab.width, crab.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data1);
    delete [] data1;
    m.sprite_two[0].set_dimensions(m.xres, m.yres);

    unsigned char *data2 = new unsigned char 
        [girl2.width * girl2.height * 4];
    for (int i=0; i<girl2.height; i++) {
        for (int j=0; j<girl2.width; j++) {
            int offset  = i*girl2.width*3 + j*3;
            int offset2 = i*girl2.width*4 + j*4;
            data2[offset2+0] = girl2.data[offset+0];
            data2[offset2+1] = girl2.data[offset+1];
            data2[offset2+2] = girl2.data[offset+2];
            data2[offset2+3] =
            ((unsigned char)girl2.data[offset+0] != 255 &&
             (unsigned char)girl2.data[offset+1] != 255 &&
             (unsigned char)girl2.data[offset+2] != 255);
        }
    }
    //sprite char
    glGenTextures(1, &m.spriteid_lvl2);
    glBindTexture(GL_TEXTURE_2D, m.spriteid_lvl2);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, girl2.width, girl2.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data2);
    delete [] data2;
    m.sprite_lvl2[0].set_dimensions(m.xres, m.yres);
}


void select_level_two() {
    m.state = STATE_LEVEL_TWO;
}
void select_start_screen2(){
    m.state = STATE_INTRO;
}

void sprite_move_right2() {
    m.sprite_two[0].pos[0] += 10;
}

void sprite_move_left2() {
    m.sprite_two[0].pos[0] -= 10;
}

void sprite_move_up2() {
    m.sprite_two[0].pos[1] += 10;
}

void sprite_move_down2() {
    m.sprite_two[0].pos[1] -= 10;
}

void physics_level_two() {
    ++m.frameno;
    if (m.frameno > 20)
        m.frameno = 1;
    //movement
    m.sprite_two[0].pos[0] += m.sprite_two[0].vel[0];
    m.sprite_two[0].pos[1] += m.sprite_two[0].vel[1];
    //boundry test
    if (m.sprite_two[0].pos[0] >= m.xres) {
        m.sprite_two[0].pos[0] = m.xres;
        m.sprite_two[0].vel[0] = 0.0;
    }
    if (m.sprite_two[0].pos[0] <= 0) {
        m.sprite_two[0].pos[0] = 0;
        m.sprite_two[0].vel[0] = 0.0;
    }
    if (m.sprite_two[0].pos[1] >= m.yres) {
        m.sprite_two[0].pos[1] = m.yres;
        m.sprite_two[0].vel[1] = 0.0;
    }
    if (m.sprite_two[0].pos[1] <= 0) {
        m.sprite_two[0].pos[1] = 0;
        m.sprite_two[0].vel[1] = 0.0;
    }
    
    Flt cx = m.xres/2.0;
    Flt cy = m.yres/2.0;
    cx = m.xres * (218.0/300.0);
    cy = m.yres * (86.0/169.0);
    Flt dx = cx - m.sprite_two[0].pos[0];
    Flt dy = cy - m.sprite_two[0].pos[1];
    Flt dist = (dx*dx + dy*dy);
    if (dist < 0.01)
        dist = 0.01; 
    m.sprite_two[0].vel[0] += (dx / dist) * m.gravity;
    m.sprite_two[0].vel[1] += (dy / dist) * m.gravity;
    m.sprite_two[0].vel[0] += ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
    m.sprite_two[0].vel[1] += ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
}
void render_level_two() {
    if (m.state == STATE_LEVEL_TWO) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, m.texid_two);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      m.yres);
            glTexCoord2f(1,0); glVertex2i(m.xres, m.yres);
            glTexCoord2f(1,1); glVertex2i(m.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

      //Draw girl sprite
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glTranslatef(m.sprite_two[0].pos[0], m.sprite_two[0].pos[1], 0.0f);
        //set alpha test
        //https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/
        //xhtml/glAlphaFunc.xml
        glEnable(GL_ALPHA_TEST);
        //transparent if alpha value is greater than 0.0
        glAlphaFunc(GL_GREATER, 0.0f);
        //Set 4-channels of color intensity
        glColor4ub(255,255,255,255);
        //
        glBindTexture(GL_TEXTURE_2D, m.spriteid_two);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-m.sprite_two[0].w, 
                                          -m.sprite_two[0].h);
            glTexCoord2f(0, 0); glVertex2f(-m.sprite_two[0].w,  
                                           m.sprite_two[0].h);
            glTexCoord2f(1, 0); glVertex2f( m.sprite_two[0].w,  
                                           m.sprite_two[0].h);
            glTexCoord2f(1, 1); glVertex2f( m.sprite_two[0].w, 
                                          -m.sprite_two[0].h);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();

        Rect r;
        unsigned int c = 0x0d8fc89;
        r.bot = m.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, c, "Level 2");
        r.bot = 20;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, "To select level type the corresponding number");
    }
}

//render_start_screen(); 

unsigned char *buildAlphaDataM(Image *img)
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


