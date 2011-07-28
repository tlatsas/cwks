/******************************************************************************
 * A 2D Landscape, uses OpenGL + GLUT                                         *
 * C Source File - main.c                                                     *
 *                                                                            *
 * Tasos Latsas - (c) 2006                                                    *
 *                                                                            *
 ******************************************************************************/
 
#include <GL/glut.h>
#include <GL/gl.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "fRGBcolors.h"

#define pi 3.14159265

static GLfloat       g_fSpin = 0.0;

void init(void);
void display(void);
void spindisplay(void);
void reshape(int, int);
void mouse(int , int , int , int );
float RandomFloat();

int main(int argc, char** argv)
{
    srand((unsigned)time(NULL));
       
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(300, 300);
    glutCreateWindow("Landscape");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMainLoop();
    
    return 0;
}

void init(void)
{
     glClearColor(RGB_BLACK, 0.0);
     glShadeModel(GL_SMOOTH);
}

void display(void)
{
     glClear(GL_COLOR_BUFFER_BIT);

     // draw sky
     glBegin(GL_POLYGON);
     
       glColor3f(RGB_BLUE);
       glVertex3f(-50.0, 50.0, 0.0);
       glVertex3f(50.0, 50.0, 0.0);
       
       glColor3f(RGB_LIGHT_BLUE);    
       glVertex3f(50.0, -10.0, 0.0);
       glVertex3f(-50.0, -10.0, 0.0);
       
     glEnd();
     glFlush();     

     //rotate functions
     glPushMatrix();

     glRotatef(g_fSpin, 0.0, 0.0, -1.0);
     
     // draw sun
     glColor3f(RGB_YELLOW);
     glBegin(GL_POLYGON);
         int i;
         float fTheta = 0.0;
         for (i = 0; i < 360; i++)
         {
             fTheta = (i * pi) / 180.0;
             glVertex3f(6.0 * cos(fTheta) - 35.0, 6.0 * sin(fTheta) + 35.0, 0.0);
         }
     glEnd();
     glFlush();
          
     // draw sunbeams
     glColor3f(RGB_YELLOW);
     glBegin(GL_LINES);
       
       // y axis beam
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-35.0, 25.0, 0.0);
              
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-35.0, 45.0, 0.0);
       
       // x axis beam
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-45.0, 35.0, 0.0);
                     
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-25.0, 35.0, 0.0);
       
       // diagonical beam 1
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-45.0, 45.0, 0.0);
                     
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-25.0, 25.0, 0.0);
       
       // diagonical beam 2
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-25.0, 45.0, 0.0);
                     
       glVertex3f(-35.0, 35.0, 0.0);
       glVertex3f(-45.0, 25.0, 0.0);
              
     glEnd();
     
     // draw moon
     glColor3f(RGB_WHITE);
     glBegin(GL_POLYGON);
         fTheta = 0.0;
         for (i = 0; i <= 360; i++)
         {
             fTheta = (i * pi) / 180.0;
             glVertex3f(4.0 * cos(fTheta) + 35.0, 4.0 * sin(fTheta) - 35.0, 0.0);
         }
     glEnd();
     glFlush();
     
     glPopMatrix();
     
     // draw earth
     glBegin(GL_POLYGON);
     
       glColor3f(RGB_BROWN4);
       glVertex3f(-50.0, -10.0, 0.0);
       glVertex3f(50.0, -10.0, 0.0);
       
       glColor3f(RGB_BROWN);
       glVertex3f(50.0, -50.0, 0.0);
       glVertex3f(-50.0, -50.0, 0.0);
       
     glEnd();
     glFlush();

     // draw road
     glColor3f(RGB_DARK_GREY);
     glBegin(GL_POLYGON);
       glColor3f(RGB_DARK_GREY);
       glVertex3f(-40.0, -50.0, 0.0);
       glVertex3f(-20.0, -50.0, 0.0);
       glColor3f(RGB_GREY20);
       glVertex3f(-28.0, -10.0, 0.0);
       glVertex3f(-30.0, -10.0, 0.0);
     glEnd();
     glFlush();
     
     // draw road lines
        // road side lines
        glBegin(GL_LINES);
          glColor3f(RGB_WHITE);
          glVertex3f(-39.0, -50.0, 0.0);
          glColor3f(RGB_GREY20);
          glVertex3f(-29.5, -10.0, 0.0);

          glColor3f(RGB_WHITE);
          glVertex3f(-21.0, -50.0, 0.0);
          glColor3f(RGB_GREY20);
          glVertex3f(-28.5, -10.0, 0.0);
        glEnd();
        glFlush();

        // road middle line
        glEnable(GL_LINE_STIPPLE);
          glLineStipple(1, 0x00FF);
          glBegin(GL_LINES);
            glColor3f(RGB_WHITE);
            glVertex3f(-30.0, -50.0, 0.0);
            glColor3f(RGB_GREY20);
            glVertex3f(-29.0, -10.0, 0.0);
          glEnd();
        glDisable(GL_LINE_STIPPLE);
        glFlush();

     // draw mountain
     glColor3f(RGB_BROWN4);
     glBegin(GL_POLYGON);
       glVertex3f(-19.0, -10.0, 0.0);
       for(i = -30; i <= 30; i++)
           glVertex3f(i + 18, (-i * i) / 25 + 27 + RandomFloat() ,0.0);
       glVertex3f(49.0, -10.0, 0.0);
     glEnd();
     glFlush();
     
     // draw small mountain
     glColor3f(RGB_BROWN4);
     glBegin(GL_POLYGON);
       glVertex3f(-50.0, -10.0, 0.0);
       for(i = -10; i <= 10; i++)
           glVertex3f(i - 43, (-i * i) / 10 + RandomFloat() ,0.0);
       glVertex3f(-30.0, -10.0, 0.0);
     glEnd();
     glFlush();
    
     glutSwapBuffers();
}

void spindisplay(void)
{
     g_fSpin = g_fSpin + 0.5;
     if (g_fSpin > 360.0)
        g_fSpin = g_fSpin - 360.0;
     glutPostRedisplay();
}

void reshape(int width, int height)
{
     glViewport(0, 0, (GLsizei)width, (GLsizei)height);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
     switch (button)
     {
            case GLUT_LEFT_BUTTON:
                 if (state == GLUT_DOWN)
                    glutIdleFunc(spindisplay);
                 break;
            
            case GLUT_RIGHT_BUTTON:
                 if (state == GLUT_DOWN)
                    glutIdleFunc(NULL);
                 break;
                 
            default:
                 break;
     }
}

float RandomFloat()
{
      float number;
      
      return( (float)rand() / RAND_MAX );
}
