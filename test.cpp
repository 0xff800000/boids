#include <GL/freeglut.h> 
#include <stdlib.h> 
#include <time.h> 
#include <vector> 
#include <chrono> 
#include <iostream> 
#include <iostream> 

const int HEIGHT = 480;
const int WIDTH = 640;
const int NB_BOIDS = 20;
const float MAX_SPEED = 100;


class Boid {
    public:
        Boid() {
            x = rand() % WIDTH;
            y = rand() % HEIGHT;
            vx = rand()  / (RAND_MAX / MAX_SPEED);
            vy = rand()  / (RAND_MAX / MAX_SPEED);

            last_call = std::chrono::high_resolution_clock::now();
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


        void compute() {

        }


        void update() {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> dt = now - last_call;
            last_call = now;
            x += vx * dt.count();
            y += vy * dt.count();
            
            // Clip on borders
            if(x > WIDTH) x = WIDTH;
            if(x < 0) x = 0;
            if(y > HEIGHT) y = HEIGHT;
            if(y < 0) y = 0;

            // Mirror vector if touching the border
            if(x == WIDTH || x == 0) vx *= -1;
            if(y == HEIGHT || y == 0) vy *= -1;
        }

    private:
        const float h = 10, w =10;
        float x, y;
        float vx, vy;
        std::chrono::time_point<std::chrono::system_clock> last_call;
};

std::vector<Boid> boid_list;


void idle()
{                      
    for(auto& boid: boid_list) {
        boid.compute();
    }
    for(auto& boid: boid_list) {
        boid.update();
    }
    glutPostRedisplay();
}

void draw_update(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
    glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0xff,0xff,0xff); 	

    for(auto& boid: boid_list) {
        boid.update_draw();
    }

	glEnd();

	glFlush();	
}



int main(int argc,char** argv)
{
    srand(time(NULL));
    for(int i=0; i<NB_BOIDS; i++) {
        Boid b;
        boid_list.push_back(b);
    }

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
