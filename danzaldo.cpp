//3350
//program: danzaldo.cpp
//author:  Dylan Anzaldo
//date:    spring 2022
//
//Medieval map and sprite movement
//images courtesy: https://www.artstation.com/artwork/xJv4WW
//                 https://www.artstation.com/artwork/PmggkZ 
//
#include <iostream>
#include <fstream>
using namespace std;
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
//for text on the screen
#include "fonts.h"

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
} background("images/castle.jpg");

class Global {
public:
    int xres, yres;
    unsigned int texid;
    Global() {
        xres = 400;
        yres = 200;
    }
} d;

int danzaldo_gameover(int health) {
    if (health <= 0) {
        return 1;
    } else {
        return 0;
    }   
}

void open_level_one() {
    glClear(GL_COLOR_BUFFER_BIT);
    Rect r;

    //OpenGL initialization
    glViewport(0, 0, d.xres, d.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, d.xres, 0, d.yres, -1, 1);
    //
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
    //allow 2D texture maps
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();

    //background 
    glGenTextures(1, &d.texid);
    glBindTexture(GL_TEXTURE_2D, d.texid);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, background.width, background.height, 
            0, GL_RGB, GL_UNSIGNED_BYTE, background.data);

    glColor3ub(255, 255, 255);
    //dark mode
    //glColor3ub(80, 80, 160);
    glBindTexture(GL_TEXTURE_2D, d.texid);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1); glVertex2i(0,      0);
        glTexCoord2f(0,0); glVertex2i(0,      d.yres);
        glTexCoord2f(1,0); glVertex2i(d.xres, d.yres);
        glTexCoord2f(1,1); glVertex2i(d.xres, 0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    r.bot = d.yres - 20;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 0, 0x00ffffff, "Dylan's Level");
}
