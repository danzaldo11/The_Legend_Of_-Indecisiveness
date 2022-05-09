// Dylan Anzaldo
// CSUB - CMPS 3350 - Software Engineering
// Group project 
// Spring 2022
// danzaldo.cpp

#include <iostream>
#include <fstream>
using namespace std;
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
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
} start("images/start_screen.png"),
  castle("images/castle.png"),
  bomb("images/bomb.png"),
  hylian("images/link.png"),
  rupee("images/rupee.png"),
  credits("images/credits.png");

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
    STATE_GAME_WIN,
    STATE_CREDITS,
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
    Sprite sprite_one[2];
    Sprite sprite_two[2];
    Sprite sprite_three[2];
    unsigned int texid_start;
    unsigned int texid_one;
    //unsigned int texid_two;
    //unsigned int texid_three;
    //unsigned int texid_four;
    unsigned int texid_credits;
    unsigned int spriteid_one;
    unsigned int spriteid_two;
    unsigned int spriteid_three;
    //the box components
    float pos[2];
    float w;
    float dir;
    int inside;
    Flt gravity;
    int frameno;
    int state;
    int health;
    int rupees;
    Global() {
        xres = 800;
        yres = 400;
        //box
        w = 20.0f;
        pos[0] = 0.0f + w;
        pos[1] = yres/2.0f;
        dir = 25.0f;
        inside = 0;
        gravity = 20.0;
        frameno = 1.0;
        state = STATE_INTRO;
        health = 100;
        rupees = 0;
    }
} d;

void init_start_screen() {
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

    //background 
    glGenTextures(1, &d.texid_start);
    glBindTexture(GL_TEXTURE_2D, d.texid_start);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, start.width, start.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, start.data);
}

void init_level_one() {
    //set up variables
    d.health = 100;
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

    unsigned char *data1 = new unsigned char 
        [bomb.width * bomb.height * 4];
    for (int i=0; i<bomb.height; i++) {
        for (int j=0; j<bomb.width; j++) {
            int offset  = i*bomb.width*3 + j*3;
            int offset2 = i*bomb.width*4 + j*4;
            data1[offset2+0] = bomb.data[offset+0];
            data1[offset2+1] = bomb.data[offset+1];
            data1[offset2+2] = bomb.data[offset+2];
            data1[offset2+3] =
            ((unsigned char)bomb.data[offset+0] != 0 &&
             (unsigned char)bomb.data[offset+1] != 0 &&
             (unsigned char)bomb.data[offset+2] != 0);
        }
    }
    //bomb sprite 
    glGenTextures(1, &d.spriteid_one);
    glBindTexture(GL_TEXTURE_2D, d.spriteid_one);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bomb.width, bomb.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data1);
    delete [] data1;
    d.sprite_one[0].set_dimensions(d.xres, d.yres);

    unsigned char *data2 = new unsigned char 
        [hylian.width * hylian.height * 4];
    for (int i=0; i<hylian.height; i++) {
        for (int j=0; j<hylian.width; j++) {
            int offset  = i*hylian.width*3 + j*3;
            int offset2 = i*hylian.width*4 + j*4;
            data2[offset2+0] = hylian.data[offset+0];
            data2[offset2+1] = hylian.data[offset+1];
            data2[offset2+2] = hylian.data[offset+2];
            data2[offset2+3] =
            ((unsigned char)hylian.data[offset+0] != 255 &&
             (unsigned char)hylian.data[offset+1] != 255 &&
             (unsigned char)hylian.data[offset+2] != 255);
        }
    }
    //sprite link
    glGenTextures(1, &d.spriteid_two);
    glBindTexture(GL_TEXTURE_2D, d.spriteid_two);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hylian.width, hylian.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data2);
    delete [] data2;
    d.sprite_two[0].set_dimensions(d.xres, d.yres);

    unsigned char *data3 = new unsigned char
        [rupee.width * rupee.height * 4];
    for (int i=0; i<rupee.height; i++) {
        for (int j=0; j<rupee.width; j++) {
            int offset  = i*rupee.width*3 + j*3;
            int offset2 = i*rupee.width*4 + j*4;
            data3[offset2+0] = rupee.data[offset+0];
            data3[offset2+1] = rupee.data[offset+1];
            data3[offset2+2] = rupee.data[offset+2];
            data3[offset2+3] =
            ((unsigned char)rupee.data[offset+0] != 0 &&
             (unsigned char)rupee.data[offset+1] != 0 &&
             (unsigned char)rupee.data[offset+2] != 0);
        }
    }
    //sprite rupee
    glGenTextures(1, &d.spriteid_three);
    glBindTexture(GL_TEXTURE_2D, d.spriteid_three);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rupee.width, rupee.height, 0,
                                   GL_RGBA, GL_UNSIGNED_BYTE, data3);
    delete [] data3;
    d.sprite_three[0].set_dimensions(d.xres, d.yres);
}

void init_game_over() {
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

    //background
    glGenTextures(1, &d.texid_start);
    glBindTexture(GL_TEXTURE_2D, d.texid_start);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, start.width, start.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, start.data);
}

void init_level_credits() {
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

    //background credits
    glGenTextures(1, &d.texid_credits);
    glBindTexture(GL_TEXTURE_2D, d.texid_credits);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, credits.width, credits.height, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, credits.data);
}

void level_select_screen() {
    Rect r;

    unsigned int c = 0x00ffffff;
    r.bot = 80;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, c, "1 - Level One");
    ggprint8b(&r, 16, c, "2 - Level Two");
    ggprint8b(&r, 16, c, "3 - Level Three");
    ggprint8b(&r, 16, c, "4 - Level Four");
    ggprint8b(&r, 16, c, "C - Credits Screen");
    ggprint8b(&r, 16, c, "To select level type the corresponding number");
}

void credits_screen() {
    Rect r;

    unsigned int c = 0x004ea4f2;
    r.bot = 200;
    r.left = 400;
    ggprint8b(&r, 16, c, "Credits Page");
    ggprint8b(&r, 16, c, "---Level One---");
    ggprint8b(&r, 16, c, "Dylan Anzaldo");
    ggprint8b(&r, 16, c, "---Level Two---");
    ggprint8b(&r, 16, c, "Mariana Lara");
    ggprint8b(&r, 16, c, "---Level Three---");
    ggprint8b(&r, 16, c, "Gisela Jimenez");
    ggprint8b(&r, 16, c, "---Level Four---");
    ggprint8b(&r, 16, c, "Madyson Steiner");
    ggprint8b(&r, 16, c, " ");
    ggprint8b(&r, 16, c, "0 - Level Select");
    ggprint8b(&r, 16, c, "To select level type the corresponding number");
}

void select_level_one() {
    d.state = STATE_LEVEL_ONE;
}

void select_start_screen() {
    d.state = STATE_INTRO;
}

void select_credits() {
    d.state = STATE_CREDITS;
}

void game_over() {
    d.state = STATE_GAME_OVER;
}

void game_win() {
    d.state = STATE_GAME_WIN;
}

void sprite_move_right() {
    d.sprite_two[0].pos[0] += 10;
}

void sprite_move_left() {
    d.sprite_two[0].pos[0] += -10;
}

void sprite_move_up() {
    d.sprite_two[0].pos[1] += 10;
}

void sprite_move_down() {
    d.sprite_two[0].pos[1] += -10;
}

void physics_level_one() {
    if (d.state == STATE_LEVEL_ONE) {
        //movement
        d.sprite_one[0].pos[0] += d.sprite_one[0].vel[0];
        d.sprite_one[0].pos[1] += d.sprite_one[0].vel[1];
        //boundry test
        if (d.sprite_one[0].pos[0] >= d.xres) {
            d.sprite_one[0].pos[0] = d.xres;
            d.sprite_one[0].vel[0] = 0.0;
        }
        if (d.sprite_one[0].pos[0] <= 0) {
            d.sprite_one[0].pos[0] = 0;
            d.sprite_one[0].vel[0] = 0.0;
        }
        if (d.sprite_one[0].pos[1] >= d.yres) {
            d.sprite_one[0].pos[1] = d.yres;
            d.sprite_one[0].vel[1] = 0.0;
        }
        if (d.sprite_one[0].pos[1] <= 0) {
            d.sprite_one[0].pos[1] = 0;
            d.sprite_one[0].vel[1] = 0.0;
        }
        //move the bomb
        Flt cx = d.xres/2.0;
        Flt cy = d.yres/2.0;
        cx = d.xres * (218.0/400.0);
        cy = d.yres * (86.0/200.0);
        Flt dx = cx - d.sprite_one[0].pos[0];
        Flt dy = cy - d.sprite_one[0].pos[1];
        Flt dist = (dx*dx + dy*dy);
        if (dist < 0.01)
            dist = 0.01; //clamp
        d.sprite_one[0].vel[0] += (dx / dist) * d.gravity;
        d.sprite_one[0].vel[1] += (dy / dist) * d.gravity;
        d.sprite_one[0].vel[0] += 
            ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
        d.sprite_one[0].vel[1] += 
            ((Flt)rand() / (Flt)RAND_MAX) * 0.5 - 0.25;
        if ((d.sprite_one[0].pos[0] == d.sprite_two[0].pos[0]) &&
            (d.sprite_one[0].pos[1] == d.sprite_two[0].pos[1])) {
            d.health -= 10;
        }
        if (d.health < 0) {
            game_over();
        }
        d.sprite_three[0].pos[0] = 400;
        d.sprite_three[0].pos[1] = 200;
        if ((d.sprite_one[0].pos[0] == d.sprite_three[0].pos[0]) &&
            (d.sprite_one[0].pos[1] == d.sprite_three[0].pos[1])) {
            d.rupees += 1;
        }
        if (d.rupees == 10) {
            game_win();
        }
    }
}

void render_start_screen() {
    if (d.state == STATE_INTRO) {
        glClear(GL_COLOR_BUFFER_BIT);     
        glColor3ub(211, 193, 253);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_start);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      d.yres);
            glTexCoord2f(1,0); glVertex2i(d.xres, d.yres);
            glTexCoord2f(1,1); glVertex2i(d.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        level_select_screen();
    }
}

void render_credits_screen() {
    if (d.state == STATE_CREDITS) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(255, 255, 255);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_credits);
        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2i(0,      0);
            glTexCoord2f(0,0); glVertex2i(0,      d.yres);
            glTexCoord2f(1,0); glVertex2i(d.xres, d.yres);
            glTexCoord2f(1,1); glVertex2i(d.xres, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        credits_screen();
    }
}

void render_level_one() {
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

        //Draw bomb sprite
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glTranslatef(d.sprite_one[0].pos[0], d.sprite_one[0].pos[1], 0.0f);
        //set alpha test
        //https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/
        //xhtml/glAlphaFunc.xml
        glEnable(GL_ALPHA_TEST);
        //transparent if alpha value is greater than 0.0
        glAlphaFunc(GL_GREATER, 0.0f);
        //Set 4-channels of color intensity
        glColor4ub(255,255,255,255);
        //
        glBindTexture(GL_TEXTURE_2D, d.spriteid_one);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-d.sprite_one[0].w, 
                                          -d.sprite_one[0].h);
            glTexCoord2f(0, 0); glVertex2f(-d.sprite_one[0].w,  
                                           d.sprite_one[0].h);
            glTexCoord2f(1, 0); glVertex2f( d.sprite_one[0].w,  
                                           d.sprite_one[0].h);
            glTexCoord2f(1, 1); glVertex2f( d.sprite_one[0].w, 
                                          -d.sprite_one[0].h);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();

        //Draw hylian sprite
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glTranslatef(d.sprite_two[0].pos[0], d.sprite_two[0].pos[1], 0.0f);
        //set alpha test
        //https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/
        //xhtml/glAlphaFunc.xml
        glEnable(GL_ALPHA_TEST);
        //transparent if alpha value is greater than 0.0
        glAlphaFunc(GL_GREATER, 0.0f);
        //Set 4-channels of color intensity
        glColor4ub(255,255,255,255);
        //
        glBindTexture(GL_TEXTURE_2D, d.spriteid_two);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-d.sprite_two[0].w, 
                                          -d.sprite_two[0].h);
            glTexCoord2f(0, 0); glVertex2f(-d.sprite_two[0].w,  
                                           d.sprite_two[0].h);
            glTexCoord2f(1, 0); glVertex2f( d.sprite_two[0].w,  
                                           d.sprite_two[0].h);
            glTexCoord2f(1, 1); glVertex2f( d.sprite_two[0].w, 
                                          -d.sprite_two[0].h);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();

        //Draw rupee sprite
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glTranslatef(d.sprite_three[0].pos[0], 
                d.sprite_three[0].pos[1], 0.0f);
        //set alpha test
        //https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/
        //xhtml/glAlphaFunc.xml
        glEnable(GL_ALPHA_TEST);
        //transparent if alpha value is greater than 0.0
        glAlphaFunc(GL_GREATER, 0.0f);
        //Set 4-channels of color intensity
        glColor4ub(255,255,255,255);
        //
        glBindTexture(GL_TEXTURE_2D, d.spriteid_three);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-d.sprite_three[0].w, 
                                          -d.sprite_three[0].h);
            glTexCoord2f(0, 0); glVertex2f(-d.sprite_three[0].w,  
                                           d.sprite_three[0].h);
            glTexCoord2f(1, 0); glVertex2f( d.sprite_three[0].w,  
                                           d.sprite_three[0].h);
            glTexCoord2f(1, 1); glVertex2f( d.sprite_three[0].w, 
                                          -d.sprite_three[0].h);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();

        Rect r;
        unsigned int c = 0x00ffff44;
        r.bot = d.yres - 20;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 0, c, "Level 1");
        r.left = d.xres - 100;
        ggprint8b(&r, 16, c, "Health: %i", d.health);
        ggprint8b(&r, 16, c, "Rupees: %i", d.rupees);
        ggprint8b(&r, 16, c, "W - Move Up");
        ggprint8b(&r, 16, c, "A - Move Left");
        ggprint8b(&r, 16, c, "S - Move Down");
        ggprint8b(&r, 16, c, "D - Move Right");
        r.left = 10;
        r.bot = 25;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c, 
                "To select level type the corresponding number");
       }
}

void render_game_over() {
    if (d.state == STATE_GAME_OVER) {
        glClear(GL_COLOR_BUFFER_BIT);     
        glColor3ub(211, 193, 253);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_start);
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
        ggprint8b(&r, 0, c, "GAME OVER");
        r.left = 10;
        r.bot = 25;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c,
                "To select level type the corresponding number");
    }
}

void render_game_win() {
    if (d.state == STATE_GAME_WIN) {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(211, 193, 253);
        //dark mode
        //glColor3ub(80, 80, 160);
        glBindTexture(GL_TEXTURE_2D, d.texid_start);
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
        ggprint8b(&r, 0, c, "GAME WIN");
        r.left = 10;
        r.bot = 25;
        ggprint8b(&r, 16, c, "0 - Level Select");
        ggprint8b(&r, 16, c,
                "To select level type the corresponding number");
    }
}
