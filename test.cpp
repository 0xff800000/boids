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
    private:
        const float h = 20, w =10;
        const float alignment_coef = 0.005, separation_coef = 0.005, cohesion_coef = 0.005;
        float visual_range = 300;
        std::complex<float> position;
        std::complex<float> velocity;
        std::complex<float> next_velocity;
        std::chrono::time_point<std::chrono::system_clock> last_call;


    public:
        Boid() {
            float x = rand() % WIDTH;
            float y = rand() % HEIGHT;
            float vx = MAX_SPEED * rand()  / RAND_MAX;
            float vy = MAX_SPEED * rand()  / RAND_MAX;
            position = std::complex<float>(x, y);
            velocity = std::complex<float>(vx, vy);
            next_velocity = std::complex<float>(vx, vy);

            last_call = std::chrono::high_resolution_clock::now();
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

            float p1_x = position.real() + v1.real();
            float p1_y = position.imag() + v1.imag();
            float p2_x = position.real() + v2.real();
            float p2_y = position.imag() + v2.imag();
            float p3_x = position.real() + v3.real();
            float p3_y = position.imag() + v3.imag();

            glBegin(GL_LINES);
            glVertex2i (p1_x,p1_y);
            glVertex2i (p2_x,p2_y);
            glVertex2i (p2_x,p2_y);
            glVertex2i (p3_x,p3_y);
            glVertex2i (p3_x,p3_y);
            glVertex2i (p1_x,p1_y);
        }


        void compute(std::vector<Boid> boids) {
            next_velocity = velocity;
            auto velocity_norm = std::complex<float>(
                    cos(std::arg(velocity)),
                    sin(std::arg(velocity))
                    );

            // Get close boids
            std::vector<Boid> close_boids;
            for(auto& boid: boids) {
                float distance = std::norm(boid.position - this->position);
                if(distance <= visual_range)
                    close_boids.push_back(boid);
            }
            int nb_boids = close_boids.size();

            // Separation


            // Alignment
            float avg_heading = 0;
            for(auto& boid: close_boids) {
                avg_heading += std::arg(boid.velocity) / nb_boids;
            }
            auto alignment_vector = std::complex<float>(cos(avg_heading), sin(avg_heading)) - velocity_norm;



            // Cohesion
            auto cg_flock = std::complex<float>(0.0, 0.0);
            for(auto& boid: close_boids) {
                cg_flock += boid.position;
            }
            cg_flock.real(cg_flock.real() / nb_boids);
            cg_flock.imag(cg_flock.imag() / nb_boids);
            float cohesion_heading = std::arg(cg_flock - this->position);
            auto cohesion_vector = std::complex<float>(cos(cohesion_heading), sin(cohesion_heading));


            next_velocity += (alignment_vector * alignment_coef) + (cohesion_vector * cohesion_coef);

        }


        void update() {
            velocity = next_velocity;
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> dt = now - last_call;
            last_call = now;
            position.real(position.real() + velocity.real() * dt.count());
            position.imag(position.imag() + velocity.imag() * dt.count());
            
            // Clip on borders
            if(position.real() > WIDTH) position.real(WIDTH);
            if(position.real() < 0) position.real(0);
            if(position.imag() > HEIGHT) position.imag(HEIGHT);
            if(position.imag() < 0) position.imag(0);

            // Mirror vector if touching the border
            if(position.real() == WIDTH || position.real() == 0)  velocity.real(velocity.real() * -1);
            if(position.imag() == HEIGHT || position.imag() == 0) velocity.imag(velocity.imag() * -1);
        }
};

std::vector<Boid> boid_list;


void idle()
{                      
    for(auto& boid: boid_list) {
        boid.compute(boid_list);
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
