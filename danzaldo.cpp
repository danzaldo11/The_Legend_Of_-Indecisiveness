// Dylan Anzaldo
// CSUB - CMPS 3350 - Software Engineering
// Group project 
// Spring 2022
// danzaldo.cpp

#include <iostream>
#include <fstream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"
#include "danzaldo.h"
#include "mlara2.h"
#include "gjimenezroja.h"
#include "msteiner.h"

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
} castle("images/castle.jpg"),
  snow("images/snow_map.png"),
  beach("images/beach_map.png");

struct Vector {
    float x,y,z;
};

enum {
    STATE_INTRO,
    STATE_LEVEL_ONE,
    STATE_LEVEL_TWO,
    STATE_LEVEL_THREE,
    STATE_LEVEL_FOUR,
    STATE_GAME_OVER,
};

typedef double Flt;
//a game object
class Bat {
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
    Bat() {
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
    Bat bats[2];
    unsigned int texid_one;
    unsigned int texid_two;
    unsigned int texid_three;
    unsigned int texid_four;
    unsigned int spriteid;
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
} d;

void init_level_one() {
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

    //background castle
    glGenTextures(1, &d.texid_one);
    glBindTexture(GL_TEXTURE_2D, d.texid_one);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, castle.width, castle.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, castle.data);
}

void init_level_two() {
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

    //background beach
    glGenTextures(1, &d.texid_two);
    glBindTexture(GL_TEXTURE_2D, d.texid_two);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, beach.width, beach.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, beach.data);
}

void init_level_three() {
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

    //background snow
    glGenTextures(1, &d.texid_three);
    glBindTexture(GL_TEXTURE_2D, d.texid_three);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, snow.width, snow.height, 0,
                               GL_RGB, GL_UNSIGNED_BYTE, snow.data);
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

    //background castle
    glGenTextures(1, &d.texid_four);
    glBindTexture(GL_TEXTURE_2D, d.texid_four);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, castle.width, castle.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, castle.data);
}

void level_select_screen() {
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

void select_level_one() {
    d.state = STATE_LEVEL_ONE;
}

void select_level_two() {
    d.state = STATE_LEVEL_TWO;
}

void select_level_three() {
    d.state = STATE_LEVEL_THREE;
}

void select_level_four() {
    d.state = STATE_LEVEL_FOUR;
}

void select_start_screen() {
    d.state = STATE_INTRO;
}

void load_level_one() {
    if (d.state == STATE_LEVEL_ONE) {  
        glClear(GL_COLOR_BUFFER_BIT);       
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_one);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      d.yres);
            glTexCoord2f(1,0); glVertex2i(d.xres, d.yres);
            glTexCoord2f(1,1); glVertex2i(d.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        Rect r;
        unsigned int c = 0x00ffff44;
        r.bot = d.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, c, "Dylan's Level");
        r.bot = 20;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, "To select level type the corresponding number");
       }
}

void load_level_two() {
    if (d.state == STATE_LEVEL_TWO) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_two);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      d.yres);
            glTexCoord2f(1,0); glVertex2i(d.xres, d.yres);
            glTexCoord2f(1,1); glVertex2i(d.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        Rect r;
        unsigned int c = 0x00ffff44;
        r.bot = d.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, c, "Mlara2's Level");
        r.bot = 20;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, "To select level type the corresponding number");
    }
}

void load_level_three() {
    if (d.state == STATE_LEVEL_THREE) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_three);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      d.yres);
            glTexCoord2f(1,0); glVertex2i(d.xres, d.yres);
            glTexCoord2f(1,1); glVertex2i(d.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        Rect r;
        unsigned int c = 0x00ffff44;
        r.bot = d.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, c, "gjimenezroja's Level");
        r.bot = 20;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, "To select level type the corresponding number");
    }
}

void load_level_four() {
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

        Rect r;
        unsigned int c = 0x00ffff44;
        r.bot = d.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 16, c, "Msteiner's Level");
        r.bot = 20;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, "To select level type the corresponding number");
    }
}
