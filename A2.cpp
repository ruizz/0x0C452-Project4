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
	Point (int _x, int _y) : x(_x), y(_y) { }
	int x;
	int y;
};

struct Car {
	Point location;
	int k11, k12, k21, k22;
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

	Point location =    Point(0, 0);
	Point front =       Point(0, -25);
	Point backLeft =    Point(-15, 15);
	Point backRight =   Point(15, 15);
	Point leftSensor =  Point(-10, -25);
	Point rightSensor = Point(10, -25);

	glPushMatrix();
	glTranslatef(c.location.x, c.location.y, 0);
	glRotatef(c.rotation, 0.0, 0.0, 1.0);

	// Draw car here.
	glColor3f (1, 1, 1);
	glBegin(GL_LINES);
	glVertex2i (0, 0);
	glVertex2i (-10, -25);
	glVertex2i (0, 0);
	glVertex2i (10, -25);
	glEnd();

	glColor3f (0.5, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2i (0, -25);
	glVertex2i (-15, 15);
	glVertex2i (15, 15);
	glEnd();

	glPopMatrix();
}


void init(void) {

	std::ifstream ifs("input.txt");
	std::string sNumOfCars = "";
	std::string sCarData = "";

	while (ifs) {
		getline(ifs, sNumOfCars);
		int numOfCars = atoi(sNumOfCars.c_str());
		for (int i = 0; i < numOfCars; i++) {
			std::string carX, carY, carR, carK11, carK12, carK21, carK22;

			getline(ifs, sCarData);
			std::stringstream carData(sCarData);
			carData >> carX >> carY >> carR >> carK11 >> carK12 >> carK21 >> carK22;
			std::cout << carX << carY << carR << carK11 << carK12 << carK21 << carK22;
			cars.push_back(Car(Point(atoi(carX.c_str()), atoi(carY.c_str())), atoi(carR.c_str()), atoi(carK11.c_str()), atoi(carK12.c_str()), atoi(carK21.c_str()), atoi(carK22.c_str())));
		}
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPointSize(10.0);

	// Set projection parameters.
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
		int w1, w2;
		//read in sensor values S1 and S2
		//matrix multiplication [S1 S2][k11 k12 k21 k22]=[w1 w2]
		Point sensor1 = Point(cars[i].location.x + 10*sin(cars[i].rotation),cars[i].location.y + 25*cos(cars[i].rotation));
		Point sensor2 = Point(cars[i].location.x - 10*sin(cars[i].rotation),cars[i].location.y + 25*cos(cars[i].rotation));
		double sensorValue1 = field[sensor1.x][sensor2.y];
		double sensorValue2 = field[sensor2.x][sensor2.y];

		w1 = sensorValue1*cars[i].k11 + sensorValue2*cars[i].k12;
		w2 = sensorValue1*cars[i].k21 + sensorValue2*cars[i].k22;

		cars[i].rotation += (w1-w2);
		int moveX = cos(cars[i].rotation)*(w1+w2);
		int moveY = sin(cars[i].rotation)*(w1+w2);
		cars[i].location.x +=moveX;
		cars[i].location.y +=moveY;
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
		if (state == GLUT_UP) {

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
