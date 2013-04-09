#include "stddef.h"
#include "glut.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>

#define PI 3.14159265

static GLfloat move = 0.0;
static GLfloat spin = 0.0;
static GLfloat offsetX = 0;
static GLfloat offsetY = 0;
int polyToggle = 3;
int startPosX = 0;
int startPosY = 0;
bool drag = false;
double field[800][600];
struct Car;
std::vector <Car> cars;

struct Point {

	Point();
	Point (double _x, double _y) : x(_x), y(_y) { }
	double x;
	double y;
};

struct Car {

	Point location;
	double k11, k12, k21, k22;
	double rotation;

	Car();
	Car(Point _location, double _rotation, int _k11, int _k12, int _k21, int _k22) 
		: location(_location), rotation(_rotation), k11(_k11), k12(_k12), k21(_k21), k22(_k22) { }
};

void calculatePoint(int x, int y) {

	for(int i=0; i<800; i++){
		for(int j=0; j<600; j++){
			double tempLight = sqrt((pow((x-i),2)+pow((y-j),2)));
			if (field[i][j] < 100/tempLight){
				field[i][j]=100/tempLight;
			}
		}
	}
	field[x][y]=100;
}

void drawPoint(int x, int y) {

	glColor3f (1,1,1);
	for(int i=0; i<800; i++){
		for(int j=0; j<600; j++){
			if (field[i][j] == 100) {
				glBegin(GL_POINTS);
				glVertex2f(i,j);
				glEnd();
			}
		}		
	}
}

void drawCar(Car c) {

	glPushMatrix();
	glTranslatef(c.location.x, c.location.y, 0);
	glRotatef(-c.rotation + 90, 0.0, 0.0, 1.0);

	// Draw car here.
	glColor3f (1, 1, 1);
	glBegin(GL_LINES);
	glVertex2i (0, 0);
	glVertex2i (-17, -17);
	glVertex2i (0, 0);
	glVertex2i (17, -17);
	glEnd();

	glColor3f (0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glVertex2i (-10, 5);
	glVertex2i (-25, 5);
	glVertex2i (-25, 25);
	glVertex2i (-10, 25);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2i (10, 5);
	glVertex2i (25, 5);
	glVertex2i (25, 25);
	glVertex2i (10, 25);
	glEnd();

	glColor3f (c.k11, c.k12, c.k21);
	glBegin(GL_POLYGON);
	glVertex2i (0, -25);
	glVertex2i (-17, 15);
	glVertex2i (17, 15);
	glEnd();

	glPopMatrix();
}

void init(void) {

	std::ifstream ifs("input.txt");
	std::string sNumOfCars = "";
	std::string sCarData = "";

	while (ifs) {
		std::string carX, carY, carR, carK11, carK12, carK21, carK22;

		getline(ifs, sCarData);
		std::stringstream carData(sCarData);
		carData >> carX >> carY >> carR >> carK11 >> carK12 >> carK21 >> carK22;
		std::cout << carX << carY << carR << carK11 << carK12 << carK21 << carK22;
		cars.push_back(Car(Point(atof(carX.c_str()), atof(carY.c_str())), atoi(carR.c_str()), atoi(carK11.c_str()), atoi(carK12.c_str()), atoi(carK21.c_str()), atoi(carK22.c_str())));
	}

	// Set projection parameters.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPointSize(2.0);

	glMatrixMode (GL_PROJECTION);
	gluOrtho2D (0.0, 0.0, 0.0, 0.0);
}

// DRAW SCREEN OBJECTS HERE
void polygon(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	drawPoint(startPosX, startPosY);

	for (int i = 0; i < cars.size(); i++ ) {
		drawCar(cars[i]);
	}

	glutSwapBuffers();
}

// Calculations all done here for car.
void moveCar(void) {

	for (int i = 0; i < cars.size(); i++ ) {
		double w1, w2;

		Point sensor1 = Point(cars[i].location.x + 25*cos(((45-cars[i].rotation)*PI)/180), cars[i].location.y + 25*sin(((45-cars[i].rotation)*PI)/180));
		Point sensor2 = Point(cars[i].location.x + 25*cos(((-45-cars[i].rotation)*PI)/180), cars[i].location.y + 25*sin(((-45-cars[i].rotation)*PI)/180));

		double sensorValue1 = field[(int)sensor1.x][(int)sensor1.y];
		double sensorValue2 = field[(int)sensor2.x][(int)sensor2.y];

		w1 = sensorValue1*cars[i].k11 + sensorValue2*cars[i].k12;
		w2 = sensorValue1*cars[i].k21 + sensorValue2*cars[i].k22;

		cars[i].rotation += (w2-w1);
		double moveX = cos(((360-cars[i].rotation)*PI)/180)*(w1+w2)/25;
		double moveY = sin(((360-cars[i].rotation)*PI)/180)*(w1+w2)/25;

		cars[i].location.x +=moveX;
		cars[i].location.y +=moveY;

		if (cars[i].location.x > 775)
			cars[i].location.x = 25;
		if (cars[i].location.x < 25)
			cars[i].location.x = 775;
		if (cars[i].location.y > 575)
			cars[i].location.y = 25;
		if (cars[i].location.y < 25)
			cars[i].location.y = 575;
	}
	glutPostRedisplay();
}

void resetLights(void) {
	for(int i=0; i<800; i++){
		for(int j=0; j<600; j++){
			field[i][j] = 0.0;
		}
	}
	glutPostRedisplay();
}

void reshape(int w, int h) {

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 800.0, 600.0, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse (int button, int state, int x, int y) {

	switch(button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			startPosX = x;
			startPosY = y;
			calculatePoint(startPosX, startPosY);
			glutIdleFunc(moveCar);
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			glutIdleFunc(resetLights);
		}
		break;
	default:
		break;
	}
}

int main (int argc, char ** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CSCE 452 P4");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	init();
	glutDisplayFunc(polygon);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}
