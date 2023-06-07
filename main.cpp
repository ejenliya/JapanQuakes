#include <cstdlib>
#include <cstdio>
#include <GL/glut.h>
#include <GL/glu.h>
#include <string>
#include "helper.h"

using namespace std;

void specialKeyboard(int c, int x, int y) {
    switch (c) {
        case GLUT_KEY_LEFT:
            zoomLeft -= zoomStep;
            zoomRight += zoomStep;
            break;
        case GLUT_KEY_RIGHT:
            zoomLeft += zoomStep;
            zoomRight -= zoomStep;
            break;
        case GLUT_KEY_UP:
            zoomBottom += zoomStep;
            zoomTop -= zoomStep;
            break;
        case GLUT_KEY_DOWN:
            zoomBottom -= zoomStep;
            zoomTop += zoomStep;
            break;
        default:
            return;
    }
}

void registerViewportTransform() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(canvasSize[0] - zoomLeft,
               canvasSize[1] + zoomRight,
               canvasSize[2] - zoomBottom,
               canvasSize[3] + zoomTop);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
}

void showJapan() {
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
    for (auto &e: geoPoints) {
        glVertex2f(e.lat, e.lon);
    }
    glEnd();
}

void showQuakesInTime() {
    glBegin(GL_LINES);
    for (auto &e: earthquakes) {
        if (e.dateTime >= startTimer && e.dateTime <= endTimer && e.mag > curMag) {
            text = e.day + '.' + e.month + '.' + e.year;
            showEarthQuake(e);
        }
    }
    glEnd();
}

void showQuakes() {
    glBegin(GL_LINES);
    for (auto &e: earthquakes) {
        if (e.mag >= curMag) {
            showEarthQuake(e);
        }
    }
    glEnd();
}

void showText() {
    glColor3f(1, 1, 1);
    glRasterPos2f(canvasSize[0] - 0.7, canvasSize[2] - 0.7);
    int len = text.size();
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void anim(int x) {
    switch (x) {
        case 0: // play default animation
            if (timeGoesForward) {
                startTimer += timeChange;
                endTimer += timeChange;
                glutPostRedisplay();
                glutTimerFunc(1, anim, 0);
            }
            break;
        case 1: //stop animation
            setDefault();
            curMag = 3;
            text = "magnitude >= 3";
            break;
        case 2: // backwards
            if (timeGoesBack) {
                startTimer -= timeChange;
                endTimer -= timeChange;
                glutPostRedisplay();
                glutTimerFunc(1, anim, 2);
            }
            break;
        case 3: // pause
            if (timeIsStopped) {
                glutPostRedisplay();
                glutTimerFunc(1, anim, 3);
            }
            break;
        default:
            return;
    }
}

void display() {
    registerViewportTransform();
    if (timeGoesForward || timeGoesBack || timeIsStopped) {
        showQuakesInTime();
    } else {
        showQuakes();
    }
    showText();
    showJapan();
    glutSwapBuffers();
}

void processSpaceButton() {
    if (timeIsStopped) {
        if (prevF) {
            animSet(true, false, false);
            glutTimerFunc(1, anim, 0);
        }
        if (prevB) {
            animSet(false, true, false);
            glutTimerFunc(1, anim, 2);
        }
    } else {
        if (timeGoesForward) {
            prevF = true;
            prevB = false;
        }
        if (timeGoesBack) {
            prevF = false;
            prevB = true;
        }
        animSet(false, false, true);
        glutTimerFunc(1, anim, 3);
    }
}

void keyboard(unsigned char c, int x, int y) {
    if (c == 27) { // escape
        exit(0);
    }
    if (isdigit(c)) {
        curMag = c - 48;
        text = "magnitude > " + to_string(curMag);
    } else {
        c = ::tolower(c);
        if (c == 'a') {
            zoom(true);
        } else if (c == 'z') {
            zoom(false);
        } else if (c == 'o') {
            animSet(false, false, false);
            glutTimerFunc(1, anim, 1);
        } else if (c == 'p' && !timeGoesBack && !timeGoesForward && !timeIsStopped) {
            animSet(true, false, false);
            startTimer = earthquakes.begin()->dateTime;
            endTimer = startTimer + secInMonth;
            curMag = 3;
            glutTimerFunc(1, anim, 0);
        } else if (c == 'g' && !timeGoesBack) {
            animSet(false, true, false);
            glutTimerFunc(1, anim, 2);
        } else if (c == 'h' && !timeGoesForward) {
            animSet(true, false, false);
            glutTimerFunc(1, anim, 0);
        } else if (c == 32) {
            processSpaceButton();
        }
    }
}

int main(int argc, char **argv) {
    readJapanMap();
    setUpWindow();
    readQuakesMap();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W_SIZE_X, W_SIZE_Y);
    glutCreateWindow("Earthquakes in Japan");

    if (glutGet(GLUT_WINDOW_COLORMAP_SIZE) != 0) {
        printf("FAIL: bad RGBA colormap size\n");
        exit(1);
    }

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMainLoop();

    return 0;
}
