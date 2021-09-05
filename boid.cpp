#include <GL/freeglut.h> 
#include <stdlib.h> 
#include <time.h> 
#include <vector> 
#include <chrono> 
#include <iostream> 
#include <complex> 
#include <numbers> 
#include <thread> 

const int HEIGHT = 1080;
const int WIDTH = 1920;
const int NB_BOIDS = 100;
const float MAX_SPEED = 200;


class Boid {
    private:
        const float h = 20, w =10;
        const float alignment_coef = 0.5, separation_coef = 0.9, cohesion_coef = 0.5;
        const float randomized_coef = 1;
        float visual_range = 1000;
        std::complex<float> position;
        std::complex<float> velocity;
        std::complex<float> next_velocity;
        std::chrono::time_point<std::chrono::system_clock> last_call;
        int id;


    public:
        Boid() {
            static int i=0;
            id = i++;
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
                if(boid.id == this->id) continue;
                float distance = std::norm(boid.position - this->position);
                if(distance <= visual_range)
                    close_boids.push_back(boid);
            }
            int nb_boids = close_boids.size();



            // Alignment
            float avg_heading = 0;
            for(auto& boid: close_boids) {
                if(boid.id == this->id) continue;
                avg_heading += std::arg(boid.velocity) / nb_boids;
            }
            auto alignment_vector = std::complex<float>(cos(avg_heading), sin(avg_heading)) - velocity_norm;



            // Cohesion
            auto cg_flock = std::complex<float>(0.0, 0.0);
            for(auto& boid: close_boids) {
                if(boid.id == this->id) continue;
                cg_flock += boid.position;
            }
            if(nb_boids != 0) cg_flock.real(cg_flock.real() / nb_boids);
            if(nb_boids != 0) cg_flock.imag(cg_flock.imag() / nb_boids);
            //std::cout << cg_flock << std::endl;
            float cohesion_heading = std::arg(cg_flock - this->position);
            auto cohesion_vector = std::complex<float>(cos(cohesion_heading), sin(cohesion_heading));


            // Separation
            auto separation_vector = std::complex<float>(0.0, 0.0);
            for(auto& boid: close_boids) {
                if(boid.id == this->id) continue;
                auto opposite = this->position - boid.position;
                float distance = std::norm(opposite);
                float weight = -(1/visual_range) * distance + 1;
                separation_vector += std::polar<float>(1.0, std::arg(opposite)) * weight;
            }
            std::vector< std::complex<float> > borders;
            borders.push_back(std::complex<float>(WIDTH, this->position.imag())); // Borders x
            borders.push_back(std::complex<float>(0.0, this->position.imag())); // Borders x
            borders.push_back(std::complex<float>(this->position.real(), HEIGHT)); // Borders y
            borders.push_back(std::complex<float>(this->position.real(), 0.0)); // Borders y
            for(auto& b: borders) {
                auto opposite = this->position - b;
                float distance = std::norm(opposite);
                if(distance > visual_range) continue;
                float weight = -(1/visual_range) * distance + 1 * 10;
                separation_vector += std::polar<float>(1.0, std::arg(opposite)) * weight;
            }

            // Randomized
            float randomized_heading = 2*std::numbers::pi * rand() / RAND_MAX;
            auto randomized_vector = std::complex<float>(cos(randomized_heading), sin(randomized_heading));



            next_velocity += (randomized_vector * randomized_coef);
            next_velocity += (separation_vector * separation_coef);
            if(nb_boids != 0) next_velocity += (alignment_vector * alignment_coef);
            if(nb_boids != 0) next_velocity += (cohesion_vector * cohesion_coef);

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
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
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
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Randomly generated points");
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glutDisplayFunc(draw_update);
	glutIdleFunc(idle);
	glutMainLoop();
	
	return 0;
}
