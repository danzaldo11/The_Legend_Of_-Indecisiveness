// Madyson Steiner
// CMPS 3350
//


#include <iostream>
#include <fstream>
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
#include "msteiner.h"

using namespace std;

// Forest source: https://rpgmaker.net/media/content/games/2584/screenshots/Map_16.png
// Fiend source: https://witcher.fandom.com/wiki/Fiend
// Druid Elf source: https://www.pngegg.com/en/png-deysa

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
}  start_screen("images/start_screen.png"),
   druid("images/DruidElf.png"),
   fiend("images/fiend_enemy.png"),
   forest("images/Forest.png");

struct Vector {
    float x,y,z;
};

enum {
    STATE_INTRO,
    STATE_LEVEL_FOUR,
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
    Sprite sprite_four[2];
    Sprite sprite_level4[2];
    unsigned int texid_start;
    unsigned int texid_four;
    unsigned int spriteid_four;
    unsigned int spriteid_level4;
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
    glTexImage2D(GL_TEXTURE_2D, 0, 3, start_screen.width, start_screen.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, start_screen.data);
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

void init_level_four() {
    //OpenGL initialization
    glViewport(0, 0, d.xres, d.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, d.xres, 0, d.yres, -1, 1);
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

    //background forest
    glGenTextures(1, &d.texid_four);
    glBindTexture(GL_TEXTURE_2D, d.texid_four);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, forest.width, forest.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, forest.data);


   unsigned char *data1 = new unsigned char 
        [fiend.width * fiend.height * 4];
    for (int i=0; i<fiend.height; i++) {
        for (int j=0; j<fiend.width; j++) {
            int offset  = i*fiend.width*3 + j*3;
            int offset2 = i*fiend.width*4 + j*4;
            data1
    [offset2+0] = fiend.data[offset+0];
            data1
    [offset2+1] = fiend.data[offset+1];
            data1
    [offset2+2] = fiend.data[offset+2];
            data1
    [offset2+3] =
            ((unsigned char)fiend.data[offset+0] != 255 &&
             (unsigned char)fiend.data[offset+1] != 255 &&
             (unsigned char)fiend.data[offset+2] != 255);
}
    }
    //sprite link
    glGenTextures(1, &s.spriteid_four);
    glBindTexture(GL_TEXTURE_2D, s.spriteid_four);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fiend.width, fiend.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data1
                            );
    delete [] data1;
    s.sprite_level4[0].set_dimensions(s.xres, s.yres);

    unsigned char *data2 = new unsigned char 
        [druid.width * druid.height * 4];
    for (int i=0; i<druid.height; i++) {
        for (int j=0; j<druid.width; j++) {
            int offset  = i*druid.width*3 + j*3;
            int offset2 = i*druid.width*4 + j*4;
            data2[offset2+0] = druid.data[offset+0];
            data2[offset2+1] = druid.data[offset+1];
            data2[offset2+2] = druid.data[offset+2];
            data2[offset2+3] =
            ((unsigned char)druid.data[offset+0] != 255 &&
             (unsigned char)druid.data[offset+1] != 255 &&
             (unsigned char)druid.data[offset+2] != 255);
        }
    }
    //sprite char
    glGenTextures(1, &s.spriteid_level4);
    glBindTexture(GL_TEXTURE_2D, s.spriteid_level4);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, druid.width, druid.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data2);
    delete [] data2;
    s.sprite_level4[0].set_dimensions(s.xres, s.yres);

}

void select_level_four() {
    s.state = STATE_LEVEL_FOUR;
}

void ssprite_move_right() {
    s.sprite_four[0].pos[0] += 1;
}

void ssprite_move_left() {
    s.sprite_four[0].pos[0] -= 1;
}

void ssprite_move_up() {
    s.sprite_four[0].pos[1] += 1;
}

void ssprite_move_down() {
    s.sprite_four[0].pos[1] -= 1;
}
void physics_level_four() {
    ++s.frameno;
    if (s.frameno > 20)
        s.frameno = 1;
    //movement
    s.sprite_four[0].pos[0] += s.sprite_four[0].vel[0];
    s.sprite_four[0].pos[1] += s.sprite_four[0].vel[1];
    //boundry test
    if (s.sprite_four[0].pos[0] >= s.xres) {
        s.sprite_four[0].pos[0] = s.xres;
        s.sprite_four[0].vel[0] = 0.0;
    }
    if (s.sprite_four[0].pos[0] <= 0) {
        s.sprite_four[0].pos[0] = 0;
        s.sprite_four[0].vel[0] = 0.0;
    }
    if (s.sprite_four[0].pos[1] >= s.yres) {
        s.sprite_four[0].pos[1] = s.yres;
        s.sprite_four[0].vel[1] = 0.0;
    }
    if (s.sprite_four[0].pos[1] <= 0) {
        s.sprite_four[0].pos[1] = 0;
        s.sprite_four[0].vel[1] = 0.0;
    }
    //move the bee toward the flower...
    Flt cx = s.xres/2.0;
    Flt cy = s.yres/2.0;
    cx = s.xres * (218.0/300.0);
    cy = s.yres * (86.0/169.0);
    Flt dx = cx - s.sprite_four[0].pos[0];
    Flt dy = cy - s.sprite_four[0].pos[1];
    Flt dist = (dx*dx + dy*dy);
    if (dist < 0.01)
        dist = 0.01; //clamp
    s.sprite_four[0].vel[0] += (dx / dist) * s.gravity;
    s.sprite_four[0].vel[1] += (dy / dist) * s.gravity;
    s.sprite_four[0].vel[0] += ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
    s.sprite_four[0].vel[1] += ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
}

void render_level_four() {
     if (d.state == STATE_LEVEL_FOUR) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_four);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      d.yres);
            glTexCoord2f(1,0); glVertex2i(d.xres, d.yres);
            glTexCoord2f(1,1); glVertex2i(d.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        //Druid sprite
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glTranslatef(s.sprite_four[0].pos[0], s.sprite_four[0].pos[1], 0.0f);
        //set alpha test
        //https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/
        //xhtml/glAlphaFunc.xml
        glEnable(GL_ALPHA_TEST);
        //transparent if alpha value is greater than 0.0
        glAlphaFunc(GL_GREATER, 0.0f);
        //Set 4-channels of color intensity
        glColor4ub(255,255,255,255);
        //
        glBindTexture(GL_TEXTURE_2D, s.spriteid_four);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-s.sprite_four[0].w, 
                                          -s.sprite_four[0].h);
            glTexCoord2f(0, 0); glVertex2f(-s.sprite_four[0].w,  
                                           s.sprite_four[0].h);
            glTexCoord2f(1, 0); glVertex2f( s.sprite_four[0].w,  
                                           s.sprite_four[0].h);
            glTexCoord2f(1, 1); glVertex2f( s.sprite_four[0].w, 
                                          -s.sprite_four[0].h);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();

        Rect r;
        unsigned int c = 0x00ffff44;
        r.bot = d.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, c, "Level 4");
        r.bot = 20;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, "To select level type the corresponding number");
    }
}

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




/* void msteiner_Check_Stamina(int stamina)
{
    //stamina = 100;

    if (stamina <= 0)
    {
        printf("You are out of breath, take a walk to heal.\n");
        stamina++;
    }
    else
    {
        printf("Stamina: %i\n", stamina);
        printf("Keep Running!\n");
        stamina --;
    }
}
*/

