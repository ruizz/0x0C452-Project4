#include <stdio.h>
#include "stddef.h"
#include "glut.h"
#include <vector>
static GLfloat spin = 0.0;
static GLfloat offsetX = 0;
static GLfloat offsetY = 0;
double PI = 3.14159265359;
int polyToggle = 3;
int startPosX = 0;
int startPosY = 0;
bool drag = false;
struct Car;
struct Light;
std::vector<Car> cars;
std::vector<Light> lights;
struct Point {

	Point();
	Point (int _x, int _y) : x(_x), y(_y) { }
	int x;
	int y;
};
struct Car{
	Point location;
	double rotation;
	int k11;
	int k12;
	int k21;
	int k22;

	double movX;
	double movY;
	Car(Point _location, double _rotation, int _k11, int _k12, int _k21, int _k22)
		:location(_location), rotation(_rotation), k11(_k11), k12(_k12), k21(_k21), k22(_k22){ movX=0; movY=0;}
};
struct Light{
	Point loc;
	int intensity;
	Light(Point _loc, int _intensity)
		:loc(_loc), intensity(_intensity) {}
};
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

	glColor3f (1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2i (0, -25);
	glVertex2i (-15, 15);
	glVertex2i (15, 15);
	glEnd();

	glPopMatrix();
}
void drawPoint(int x, int y)
{
	//printf("called\n");
	glBegin( GL_POINTS );
	glColor3f( 0, 1, 0 );
	glVertex2f(x,y);
	glEnd();

}
void CalcMovVec(Car c)
{
	double mx=0;
	double my=0;
	double dist = sqrt(10*10 + 25*25);
	//c.rotation=90;
	double rot = -c.rotation + PI/2.0;
	double angle = atan(10.0/25.0) + rot/180.0*PI;
	double angle2 = -atan(10.0/25.0) + rot/180.0*PI;

	
	Point leftSloc = Point(c.location.x-dist*(sin(angle) ), c.location.y-dist*cos(angle));
	Point rightSloc = Point(c.location.x-dist*(sin(angle2) ), c.location.y-dist*cos(angle2));

	/*
	printf("%f\n", angle*180/PI);
	printf("%f\n", angle2*180/PI);
	printf("%f %f\n", sin(angle), cos(angle));
	printf("Car pos %d %d\n", c.location.x, c.location.y);
	printf("Left %d %d, Right \n",leftSloc.x, leftSloc.y);
	printf("Left %d %d, Right \n",leftSloc.x, leftSloc.y);
	*/

	drawPoint(leftSloc.x, leftSloc.y);
	drawPoint(rightSloc.x, rightSloc.y);

	
	
	for(int i=0; i<lights.size(); i++)
	{
		double distl = sqrt(pow((double)(lights[i].loc.x - leftSloc.x), 2.0) + pow((double)(lights[i].loc.y - leftSloc.y), 2.0));
		double distr = sqrt(pow((double)(lights[i].loc.x - rightSloc.x), 2.0) + pow((double)(lights[i].loc.y - rightSloc.y), 2.0));
		double leftAngle = asin((double)(-lights[i].loc.y + leftSloc.y)/distl);	
		double rightAngle = asin((double)(-lights[i].loc.y + rightSloc.y)/distr);
		
		if(lights[i].loc.x < leftSloc.x)
			leftAngle = PI - leftAngle;
		if(lights[i].loc.x < rightSloc.x)
			rightAngle = PI - rightAngle;

		printf("%f\n", leftAngle*180/PI);
		printf("%f\n", rightAngle*180/PI);
		mx+=((lights[i].intensity/distl)*cos(leftAngle)+(lights[i].intensity/distr)*cos(rightAngle))/2.0;
		my-=((lights[i].intensity/distl)*sin(leftAngle)+(lights[i].intensity/distr)*sin(rightAngle))/2.0;
	}
	printf("Pointing %f %f\n", mx, my);
	
	glColor3f (0, 0, 1);
	glLineWidth(4);
	glBegin(GL_LINES);
	glVertex2i (c.location.x, c.location.y);
	glVertex2i (c.location.x + mx*10, c.location.y + my*10);
	glEnd();
}
void drawLight(Light l)
{
	//printf("called\n");
	glBegin( GL_POINTS );
	int light_size = 2;
	double unit = (double)(l.intensity/(light_size*2+1));
	/*
	for(int i=-light_size; i<=light_size; i++)
	{
		for(int j=-light_size; j<=light_size; j++)
		{
			double dist = sqrt(i*i + j*j);
			printf("%d %d %f\n", i,j,dist);
			//glColor3f( (double)l.intensity/(dist*100), (double)l.intensity/(dist*100), (double)l.intensity/(dist*100) );
			glColor3f( (double)(l.intensity - dist*unit)/100.0, (double)(l.intensity - dist*unit)/100.0, (double)(l.intensity - dist*unit)/100.0 );
			glVertex2f(l.loc.x+i,l.loc.y+j);
		}
	}
	*/
	glColor3f( (double)l.intensity/100.0, (double)l.intensity/100.0, (double)l.intensity/100.0 );
	glVertex2f(l.loc.x,l.loc.y);
	glEnd();
	//exit(1);
}

void init(void) {
	cars.push_back(Car(Point(100,200), 90, 1, 0, 0, 1));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPointSize(6.0);

	// Set projection parameters.
	glMatrixMode (GL_PROJECTION);
	gluOrtho2D (0.0, 0.0, 0.0, 0.0);
}

// DRAW SCREEN OBJECTS HERE
void polygon(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	// This is where you draw ANY car. Orgin in top-left corner.
	for(int i=0; i<lights.size(); i++)
	{
		drawLight(lights[i]);
	}
	
	// drawCar(x, y, degree, red, green, blue);
	for(int i=0; i<cars.size(); i++)
	{
		drawCar(cars[i]);
		CalcMovVec(cars[i]);
	}
	
	glutSwapBuffers();
}


void spinDisplayL(void) {

	spin = spin + 0.5;
	if (spin > 360.0)
		spin = spin-360.0;
	glutPostRedisplay();
}

void spinDisplayR(void) {

	spin = spin - 0.5;
	if (spin < -360.0)
		spin = spin+360.0;
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
			lights.push_back(Light(Point(x,y), 100));
			glutIdleFunc(spinDisplayL);
			
		}
		if (state == GLUT_UP) {
			startPosX = 0;
			startPosY = 0;
			//glutIdleFunc(NULL);
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			startPosX = x;
			startPosY = y;
			glutIdleFunc(spinDisplayR);
		}
		if (state == GLUT_UP) {
			startPosX = 0;
			startPosY = 0;
			glutIdleFunc(NULL);
		}
		break;
	default:
		break;
	}
}


// Request double buffer polygon mode.
// Register mouse input callback functions.

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
