#include <GL/freeglut.h> 
#include <stdlib.h> 
#include <time.h> 
#include <vector> 
#include <chrono> 
#include <iostream> 
#include <complex> 
#include <numbers> 

const int HEIGHT = 480;
const int WIDTH = 640;
const int NB_BOIDS = 20;
const float MAX_SPEED = 100;


class Boid {
    public:
        Boid() {
            x = rand() % WIDTH;
            y = rand() % HEIGHT;
            float vx = MAX_SPEED * rand()  / RAND_MAX;
            float vy = MAX_SPEED * rand()  / RAND_MAX;
            velocity = std::complex<float>(vx, vy);

            last_call = std::chrono::high_resolution_clock::now();
        }


        Boid(float x, float y) : x(x), y(y) {

        }


        void update_draw() {
            std::complex<float> v1(0.0, h/2.0);
            std::complex<float> v2(w/2.0, -h/2.0);
            std::complex<float> v3(-w/2.0, -h/2.0);

            std::complex<float> rotation(
                    cos(std::arg(velocity) - std::numbers::pi / 2.0),
                    sin(std::arg(velocity) - std::numbers::pi / 2.0)
                    );

            v1 *= rotation;
            v2 *= rotation;
            v3 *= rotation;

            float p1_x = x + v1.real();
            float p1_y = y + v1.imag();
            float p2_x = x + v2.real();
            float p2_y = y + v2.imag();
            float p3_x = x + v3.real();
            float p3_y = y + v3.imag();

            glBegin(GL_LINES);
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
            x += velocity.real() * dt.count();
            y += velocity.imag() * dt.count();
            
            // Clip on borders
            if(x > WIDTH) x = WIDTH;
            if(x < 0) x = 0;
            if(y > HEIGHT) y = HEIGHT;
            if(y < 0) y = 0;

            // Mirror vector if touching the border
            if(x == WIDTH || x == 0) velocity.real(velocity.real() * -1);
            if(y == HEIGHT || y == 0)velocity.imag(velocity.imag() * -1);
        }

    private:
        const float h = 20, w =10;
        float x, y;
        std::complex<float> velocity;
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
