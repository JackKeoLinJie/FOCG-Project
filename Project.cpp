//	Note: this program is done by using VSCode and OpenGL freeglut library (newer version of glut)
//	https://youtu.be/8Qkpaewj-7Y (how to setup freeglut)
//	https://jmeubank.github.io/tdm-gcc/ (C++ compiler)

#include <windows.h>
#include "objloader.h"	// required for loading Bendy's Head
#include "imageloader.cpp" // required for loading texture

std::string model_name = "Model/BendyHead3.obj";

float pos_x, pos_y, pos_z;
float angle_x = 0.0f, angle_y = 0.0f;

int x_old = 0, y_old = 0;
int current_scroll = 5;
float zoom_per_scroll;

bool is_holding_mouse = false;
bool is_updated = false;

float angleLArm = 0.0f, angleRArm = 0.0f;
float angleLHand = 0.0f, angleRHand = 0.0f;
float angleLLeg = 0.0f, angleRLeg = 0.0f;
float angleLFinger = 0.0f, angleRFinger = 0.0f;

float fingerLY=0.0f;
float fingerRY=0.0f;

Model model;

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId);
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             GL_RGB,
	             image->width, image->height,
	             0,
	             GL_RGB,
	             GL_UNSIGNED_BYTE,
	             image-> pixels);
	return textureId;
}

GLuint _textureId1, _textureId2, _textureId3;

void init() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	model.load(model_name.c_str());
	pos_x = model.pos_x;
	pos_y = model.pos_y;
	pos_z = model.pos_z - 1.0f;
	zoom_per_scroll = -model.pos_z / 10.0f;
	Image* image1 = loadBMP("Texture\\Wall.bmp");
	_textureId1 = loadTexture(image1);
	delete image1;
	Image* image2 = loadBMP("Texture\\Ceiling.bmp");
	_textureId2 = loadTexture(image2);
	delete image2;
	Image* image3 = loadBMP("Texture\\Floor.bmp");
	_textureId3 = loadTexture(image3);
	delete image3;
}

void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a windows of zero width)
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	// Reset Matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void drawHalfSphere(int scalex, int scaley, GLfloat r) {
	int i, j;
	GLfloat v[scalex*scaley][3];
	for (i=0; i<scalex; ++i) {
		for (j=0; j<scaley; ++j) {
			v[i*scaley+j][0]=r*cos(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));
			v[i*scaley+j][1]=r*sin(i*M_PI/(2*scalex));
			v[i*scaley+j][2]=r*sin(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));
		}
	}
	glBegin(GL_POLYGON);
	for (i=0; i<scalex-1; ++i) {
		for (j=0; j<scaley; ++j) {
			glVertex3fv(v[i*scaley+j]);
			glVertex3fv(v[i*scaley+(j+1)%scaley]);
			glVertex3fv(v[(i+1)*scaley+(j+1)%scaley]);
			glVertex3fv(v[(i+1)*scaley+j]);
		}
	}
	glEnd();
}

void body() {
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(0.0f, -1.3f, 0.0f);
	glScalef(0.85f, 1.3f, 0.85f);
	glutSolidSphere(2.25f, 50, 50);
	glPopMatrix();
}

void bowtie() {
	glColor3f(1.0f, 0.66f, 0.17f);
	glPushMatrix();
	glTranslatef(0.0f, 0.8f, 1.5f);
	glScalef(1.0f, 1.0f, 0.5f);
	glutSolidSphere(0.35f, 20, 20);
	glPushMatrix();
	glTranslatef(-1.5f, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(0.75f, 1.0f, 1.0f);
	glutSolidCone(0.35f, 2.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.5f, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(0.75f, 1.0f, 1.0f);
	glutSolidCone(0.35f, 2.0f, 20, 20);
	glPopMatrix();
	glPopMatrix();
}

void rightHand() {
	//wrist
	glColor3f(1.0f, 0.66f, 0.17f);
	glPushMatrix();
	glTranslatef(-6.3f, 0.75f, 0.0f);
	glRotatef(angleRHand, 0.0f, 0.0f, 1.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(0.15f, 0.35f, 20, 20);
	//palm
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.75f);
	glScalef(1.75f, 1.0f, 1.6f);
	glutSolidSphere(0.45f, 20, 20);
	glPopMatrix();
	//dot1 on back of palm
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(0.225f, 0.3f, -0.75f);
	glScalef(1.0f, 1.0f, 1.5f);
	glutSolidSphere(0.2f, 20, 20);
	glPopMatrix();
	//dot2 on back of palm
	glPushMatrix();
	glTranslatef(-0.225f, 0.3f, -0.75f);
	glScalef(1.0f, 1.0f, 1.5f);
	glutSolidSphere(0.2f, 20, 20);
	glPopMatrix();
	//pinky finger
	glColor3f(1.0f, 0.66f, 0.17f);
	glPushMatrix();
	glTranslatef(-0.55f, 0.0f, -1.5f);
	glRotatef(110.0f, 0.0f, 1.0f, 0.0f);

	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	//middle finger
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.75f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	//index finger
	glPushMatrix();
	glTranslatef(0.55f, 0.0f, -1.5f);
	glTranslatef(0.0f, fingerRY, 0.0f);
	glRotatef(70.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(angleRFinger, 0.0f, 0.0f, -1.0f);
	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	//thumb
	glPushMatrix();
	glTranslatef(-0.8f, 0.0f, -0.55f);
	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	glPopMatrix();
}

void rightArm() {
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(-1.25f, 0.75f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidCylinder(0.35f, 5.2f, 20, 20); //freeglut cylinder function
	glPopMatrix();
}

void leftHand() {
	//wrist
	glColor3f(1.0f, 0.66f, 0.17f);
	glPushMatrix();
	glTranslatef(6.25f, 0.75f, 0.0f);
	glRotatef(angleLHand, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(0.15f, 0.35f, 20, 20);
	//palm
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.75f);
	glScalef(1.75f, 1.0f, 1.6f);
	glutSolidSphere(0.45f, 20, 20);
	glPopMatrix();
	//dot1 on back of palm
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(0.225f, 0.3f, -0.75f);
	glScalef(1.0f, 1.0f, 1.5f);
	glutSolidSphere(0.2f, 20, 20);
	glPopMatrix();
	//dot2 on back of palm
	glPushMatrix();
	glTranslatef(-0.225f, 0.3f, -0.75f);
	glScalef(1.0f, 1.0f, 1.5f);
	glutSolidSphere(0.2f, 20, 20);
	glPopMatrix();
	//pinky finger
	glColor3f(1.0f, 0.66f, 0.17f);
	glPushMatrix();
	glTranslatef(-0.55f, 0.0f, -1.5f);
	glTranslatef(0.0f, fingerLY, 0.0f);
	glRotatef(110.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(angleLFinger, 0.0f, 0.0f, 1.0f);
	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	//middle finger
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.75f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	//index finger
	glPushMatrix();
	glTranslatef(0.55f, 0.0f, -1.5f);
	glRotatef(70.0f, 0.0f, 1.0f, 0.0f);
	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	//thumb
	glPushMatrix();
	glTranslatef(0.8f, 0.0f, -0.55f);
	glScalef(2.5f, 1.0f, 1.0f);
	glutSolidSphere(0.3f, 30, 30);
	glPopMatrix();
	glPopMatrix();
}

void leftArm() {
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(1.0f, 0.75f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidCylinder(0.35f, 5.2f, 20, 20); //freeglut cylinder function
	glPopMatrix();
}

void rightLeg() {
	//leg
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(-1.0f, -3.0f, 0.0f);
	glRotatef(angleRLeg, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	glutSolidCylinder(0.35f, 4.5f, 20, 20); //freeglut cylinder function
	//boot
	glColor3f(0.15f, 0.13f, 0.12f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 3.5f);
	glutSolidCylinder(0.45f, 1.5f, 20, 20); //freeglut cylinder function
	glPopMatrix();
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -5.0f, 1.0f);
	glScalef(0.7f, 1.1f, 1.0f);
	drawHalfSphere(20, 20, 1.0f);
	glPopMatrix();
	glPopMatrix();
}

void leftLeg() {
	//leg
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(1.0f, -3.0f, 0.0f);
	glRotatef(angleLLeg, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	glutSolidCylinder(0.35f, 4.5f, 20, 20); //freeglut cylinder function
	//boot
	glColor3f(0.15f, 0.13f, 0.12f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 3.5f);
	glutSolidCylinder(0.45f, 1.5f, 20, 20); //freeglut cylinder function
	glPopMatrix();
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -5.0f, 1.0f);
	glScalef(0.7f, 1.1f, 1.0f);
	drawHalfSphere(20, 20, 1.0f);
	glPopMatrix();
	glPopMatrix();
}

void bendyLower() {
	glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
	glScalef(1.66f, 1.66f, 1.66f);
	body();
	bowtie();
	glPushMatrix();
	glRotatef(angleRArm, 0.0f, 0.0f, 1.0f);
	rightArm();
	rightHand();
	glPopMatrix();
	glPushMatrix();
	glRotatef(angleLArm, 0.0f, 0.0f, 1.0f);
	leftArm();
	leftHand();
	glPopMatrix();
	rightLeg();
	leftLeg();
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}


void background() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(pos_x, pos_y, pos_z);
	glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
	glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
	glScalef(16.0f, 7.0f, 16.0f);
	glTranslatef(0.0f, 3.25f, 0.0f);
	//front side
	glBegin (GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8.0f, -8.0f, 8.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-8.0f, 8.0f, 8.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(8.0f, 8.0f, 8.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(8.0f, -8.0f, 8.0f);
	glEnd();
	//left side
	glBegin (GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8.0f, -8.0f, -8.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-8.0f, 8.0f, -8.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-8.0f, 8.0f, 8.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-8.0f, -8.0f, 8.0f);
	glEnd();
	//right side
	glBegin (GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(8.0f, -8.0f, -8.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(8.0f, 8.0f, -8.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(8.0f, 8.0f, 8.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(8.0f, -8.0f, 8.0f);
	glEnd();
	//back side
	glBegin (GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8.0f, -8.0f, -8.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-8.0f, 8.0f, -8.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(8.0f, 8.0f, -8.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(8.0f, -8.0f, -8.0f);
	glEnd();
	//top side
	glBindTexture(GL_TEXTURE_2D, _textureId2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBegin (GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8.0f, 8.0f, -8.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-8.0f, 8.0f, 8.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(8.0f, 8.0f, 8.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(8.0f, 8.0f, -8.0f);
	glEnd();
	//bottom side
	glBindTexture(GL_TEXTURE_2D, _textureId3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBegin (GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-8.0f, -8.0f, -8.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-8.0f, -8.0f, 8.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(8.0f, -8.0f, 8.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(8.0f, -8.0f, -8.0f);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawBendy() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glPushMatrix();
	glTranslatef(pos_x, pos_y, pos_z);
	glScalef(2.5f, 2.5f, 2.5f);
	glColor3f (1.0f, 1.0f, 1.0f);
	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat diffuseColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat specularColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_position[] = {1.0f, 0.5f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
	glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
	model.draw();
	bendyLower();
	glPopMatrix();
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	drawBendy();
	background();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
		case 'a':
			if(angleLArm > -25) {
				angleLArm -= 5.0f;
			}
			break;
		case 'A':
			if(angleLArm < 15) {
				angleLArm += 5.0f;
			}
			break;
		case 'd':
			if(angleRArm < 25) {
				angleRArm += 5.0f;
			}
			break;
		case 'D':
			if(angleRArm > -15) {
				angleRArm -= 5.0f;
			}
			break;
		case 'q':
			if(angleLHand > -30) {
				angleLHand -= 5.0f;
			}
			break;
		case 'Q':
			if(angleLHand < 30) {
				angleLHand += 5.0f;
			}
			break;
		case 'e':
			if(angleRHand < 30) {
				angleRHand += 5.0f;
			}
			break;
		case 'E':
			if(angleRHand > -30) {
				angleRHand -= 5.0f;
			}
			break;
		case 'z':
			if(angleLLeg > -60) {
				angleLLeg -= 5.0f;
			}
			break;
		case 'Z':
			if(angleLLeg < 60) {
				angleLLeg += 5.0f;
			}
			break;
		case 'c':
			if(angleRLeg > -60) {
				angleRLeg -= 5.0f;
			}
			break;
		case 'C':
			if(angleRLeg < 60) {
				angleRLeg += 5.0f;
			}
			break;
		case 'n':
			if(angleLFinger > -20) {
				angleLFinger -= 5.0f;
				fingerLY -= 0.05f;
			}
			break;
		case 'N':
			if(angleLFinger < 0) {
				angleLFinger += 5.0f;
				fingerLY += 0.05f;
			}
			break;
		case 'm':
			if(angleRFinger < 20) {
				angleRFinger += 5.0f;
				fingerRY -= 0.05f;
			}
			break;
		case 'M':
			if(angleRFinger > 0) {
				angleRFinger -= 5.0f;
				fingerRY += 0.05f;
			}
			break;
		case 'p':
			PlaySound(TEXT("Sound\\BATDR.wav"), NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
			break;
		case 'P':
			PlaySound(NULL, NULL, SND_ASYNC);
			break;
		case 27:
			exit(0);
			break;
	}
}

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(INTERVAL, timer, 0);
}

void mouse(int button, int state, int x, int y) {
	is_updated = true;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			x_old = x;
			y_old = y;
			is_holding_mouse = true;
		} else
			is_holding_mouse = false;
	} else if (state == GLUT_UP) {
		switch (button) {
			case 3:
				if (current_scroll > 0) {
					current_scroll--;
					pos_z += zoom_per_scroll;
				}
				break;
			case 4:
				if (current_scroll < 15) {
					current_scroll++;
					pos_z -= zoom_per_scroll;
				}
				break;
		}
	}
}

void motion(int x, int y) {
	if (is_holding_mouse) {
		is_updated = true;
		angle_y += (x - x_old);
		x_old = x;
		if (angle_y > 360.0f)
			angle_y -= 360.0f;
		else if (angle_y < 0.0f)
			angle_y += 360.0f;
		angle_x += (y - y_old);
		y_old = y;
		if (angle_x > 35.0f)
			angle_x = 35.0f;
		else if (angle_x < -15.0f)
			angle_x = -15.0f;
	}
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	int POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
	int POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
	glutInitWindowPosition(POS_X, POS_Y);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Project - Bendy");
	init();
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}