#include <GL/freeglut.h> 
#include <stdlib.h> 
#include <time.h> 
#include <vector> 

const int HEIGHT = 480;
const int WIDTH = 640;


class Boid {
    public:
        Boid() {
            x = rand() % WIDTH;
            y = rand() % HEIGHT;
        }

        Boid(float x, float y) : x(x), y(y) {

        }

        void update_draw() {
            glBegin(GL_LINES);
            float p1_x, p1_y, p2_x, p2_y, p3_x, p3_y;
            p1_x = x; p1_y = y + h/2;
            p2_x = p1_x + w/2; p2_y = p1_y - h;
            p3_x = p2_x - w; p3_y = p2_y;
            glVertex2i (p1_x,p1_y);
            glVertex2i (p2_x,p2_y);
            glVertex2i (p2_x,p2_y);
            glVertex2i (p3_x,p3_y);
            glVertex2i (p3_x,p3_y);
            glVertex2i (p1_x,p1_y);
        }
    private:
        const float h = 10, w =10;
        float x, y;
        float vx, vy;
};


void idle()
{                      
	glutPostRedisplay();
}

Boid boid(100, 100);
Boid boid1(200, 200);
Boid boid2;

void draw_update(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
    glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0xff,0xff,0xff); 	

    boid.update_draw();
    boid1.update_draw();
    boid2.update_draw();
	glEnd();

	glFlush();	
}


std::vector<Boid> boid_list;


int main(int argc,char** argv)
{
    srand(time(NULL));
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Randomly generated points");
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glutDisplayFunc(draw_update);
	glutIdleFunc(idle);
	glutMainLoop();
	
	return 0;
}
