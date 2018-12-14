//g++ -std=c++11 tp5.cpp Textuglut -lGL -lGLU -lfreeimage -o puntos

#include <iostream>
#include <fstream>
#include <sstream>
#define GLUT_DISABLE_ATEXIT_HACK	
#include <math.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <GL/glut.h>
#include "mapa.h"
#include "TextureManager.h"
using namespace std;



#define RED 1
#define GREEN 1
#define BLUE 1
#define ALPHA 1

#define ECHAP 27

GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height);
GLvoid window_key(unsigned char key, int x, int y);


//variables para el gizmo
float delta_x = -50.0; 
float delta_y = 5.0;
float mouse_x, mouse_y;
float var_x = 0.0;
float var_z = -30.0;
float var_y = 0.0;
float step = 0; //0.0 Posicion inicial. 1.0 Traslacion. 2.0 Primera Rotacion(en y). 3.0 Segunda Rotacion (en x) 4.0 Ultima Rotacion (en z)


///////////////////////////////////////////////////////////////////////////////
//(1)
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		var_z += 0.5;
		glutPostRedisplay();
		break;

	case GLUT_KEY_DOWN:
		var_z -= 0.5;
		glutPostRedisplay();
		break;

	case GLUT_KEY_LEFT:
		var_x += 0.5;
		glutPostRedisplay();
		break;

	case GLUT_KEY_RIGHT:
		var_x -= 0.5;
		glutPostRedisplay();
		break;

	case GLUT_KEY_F1:
		//step++;
		var_y -= 0.5;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F2:
		//step--;
		var_y += 0.5;
		glutPostRedisplay();
		break;

	}
}



///////////////////////////////////////////////////////////////////////////////
//(2)
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		mouse_x = x;
		mouse_y = y;
	}
}

///////////////////////////////////////////////////////////////////////////////
//(3)
///////////////////////////////////////////////////////////////////////////////
GLvoid callback_motion(int x, int y)
{
	
	delta_x += x - mouse_x;
	delta_y += y - mouse_y;
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}


GLvoid window_idle();

GLfloat Light0Pos[] = {0.0f, 1000.0f, 5000.0f, 0.0f};


Terreno tp;

void setupLights() {
  GLfloat Light0Amb[] = {0.75f, 0.75f, 0.75f, 1.0f};
  GLfloat Light0Dif[] = {0.9f, 0.9f, 0.9f, 1.0f};
  GLfloat Light0Spec[] = {0.4f, 0.4f, 0.4f, 1.0f};

  glLightfv(GL_LIGHT0, GL_AMBIENT, Light0Amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Dif);
  glLightfv(GL_LIGHT0, GL_SPECULAR, Light0Spec);
  glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);

  // Activate light.
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

GLvoid initGL()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glClearColor(RED, GREEN, BLUE, ALPHA);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  	glEnable(GL_BLEND);

setupLights();
}


void Gizmo3D(){


	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);///ROJO X
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(3.0f, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); ///VERDE Y
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 3.0f, 0.0f);
	glEnd();


	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);///AZUL Z
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 3.0f);
	glEnd();


	return;

}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);


	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("TP 5");

	initGL();
    
    tp.texture = TextureManager::Inst()->LoadTexture("fontvieille.tga", GL_BGR_EXT, GL_RGB);
    cout<<"texture: "<<tp.texture<<endl;	
    tp.load("fontvieille.txt");
	

	glutDisplayFunc(&window_display);
	glutReshapeFunc(&window_reshape);
	
	
	glutMouseFunc(&callback_mouse);
	glutMotionFunc(&callback_motion);

	glutKeyboardFunc(&window_key);
	glutSpecialFunc(&callback_special);
	
	//function called on each frame
	glutIdleFunc(&window_idle);

	glutMainLoop();
	return 1;
}

GLvoid window_display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(45.0f, 1.0f, 0.01f, 100.0f);
	glTranslatef(var_x, var_y, var_z);
	glRotatef(delta_x, 0.0, 1.0, 0.0);
	glRotatef(delta_y, 1.0, 0.0, 0.0);

	Gizmo3D();
	tp.display();

	glutSwapBuffers();

	glFlush();
}

GLvoid window_reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
}

GLvoid window_key(unsigned char key, int x, int y)
{
	switch (key) {
	case ECHAP:
		exit(1);
		break;
	default:
		printf("La touche %d non active.\n", key);
		break;
	}
}


//function called on each frame
GLvoid window_idle()
{
	glutPostRedisplay();
}




