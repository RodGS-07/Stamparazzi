#include <SDL2/SDL.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>

float x_cam = 0.0f, y_cam = 0.0f, z_cam = 0.0f, cam_yaw = 0.0f, cam_pitch = 0.0f;
bool mouse_in = false;

SDL_Window* window;

void drawCube() {
    glBegin(GL_QUADS);

    // Frente (vermelha)
    glColor3f(1, 0, 0);
    glVertex3f(-1, -1,  1);
    glVertex3f( 1, -1,  1);
    glVertex3f( 1,  1,  1);
    glVertex3f(-1,  1,  1);

    // Trás (verde)
    glColor3f(0, 1, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1,  1, -1);
    glVertex3f( 1,  1, -1);
    glVertex3f( 1, -1, -1);

    // Esquerda (azul)
    glColor3f(0, 0, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1,  1);
    glVertex3f(-1,  1,  1);
    glVertex3f(-1,  1, -1);

    // Direita (amarelo)
    glColor3f(1, 1, 0);
    glVertex3f(1, -1, -1);
    glVertex3f(1,  1, -1);
    glVertex3f(1,  1,  1);
    glVertex3f(1, -1,  1);

    // Topo (ciano)
    glColor3f(0, 1, 1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1,  1);
    glVertex3f( 1, 1,  1);
    glVertex3f( 1, 1, -1);

    // Base (magenta)
    glColor3f(1, 0, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f( 1, -1, -1);
    glVertex3f( 1, -1,  1);
    glVertex3f(-1, -1,  1);

    glEnd();
}

void prende_camera(){
	if(cam_yaw < 0.0f) cam_yaw += 360.0f;
	if(cam_yaw > 360.0f) cam_yaw -= 360.0f;
	if(cam_pitch > 89.9f) cam_pitch = 89.9f;
	if(cam_pitch < -89.9f) cam_pitch = -89.9f;
}

void move_camera(float dist, float dir, float val = 0.0f){
	if(dir >= 0.0f){
		float rad = (cam_yaw + dir) * M_PI / 180.0f;
		x_cam -= sin(rad) * dist;
		z_cam -= cos(rad) * dist;
	} else 
		y_cam += dist * val;
}

void controle_camera(float move_vel, float mouse_vel){
	if(mouse_in){
		int midx = 320, midy = 240, tempx, tempy;
		SDL_ShowCursor(SDL_DISABLE);
		SDL_GetMouseState(&tempx, &tempy);
		cam_yaw += mouse_vel * (midx - tempx);
		cam_pitch += mouse_vel * (midy - tempy);
		prende_camera();
		SDL_WarpMouseInWindow(window,midx,midy);
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if(state[SDL_SCANCODE_UP] or state[SDL_SCANCODE_W])
			if(cam_pitch != 90.0f and cam_pitch != -90.0f)
				move_camera(move_vel,0.0f);
		if(state[SDL_SCANCODE_DOWN] or state[SDL_SCANCODE_S])
			if(cam_pitch != 90.0f and cam_pitch != -90.0f)
				move_camera(move_vel,180.0f);
		if(state[SDL_SCANCODE_LEFT] or state[SDL_SCANCODE_A])
			move_camera(move_vel,90.0f);
		if(state[SDL_SCANCODE_RIGHT] or state[SDL_SCANCODE_D])
			move_camera(move_vel,270.0f);
		if(state[SDL_SCANCODE_LSHIFT] or state[SDL_SCANCODE_RSHIFT])
			if(cam_pitch != 90.0f and cam_pitch != -90.0f)
				move_camera(move_vel,-1.0f,1.0f);
		if(state[SDL_SCANCODE_LCTRL] or state[SDL_SCANCODE_RCTRL])
			if(cam_pitch != 90.0f and cam_pitch != -90.0f)
				move_camera(move_vel,-1.0f,-1.0f);
	}
	glRotatef(-cam_pitch, 1.0, 0.0, 0.0);
	glRotatef(-cam_yaw, 0.0, 1.0, 0.0);
}

void atualiza_camera_posicao(){
	glTranslatef(-x_cam,-y_cam,-z_cam);
}

int main(int argc, char* argv[]) {
    // Inicializa SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro ao inicializar SDL2: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Cria a janela com contexto OpenGL
    window = SDL_CreateWindow("Exemplo SDL2 + OpenGL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Erro ao criar janela: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Erro ao criar contexto OpenGL: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Configuração básica do OpenGL
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 800.0/600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

	SDL_ShowCursor(SDL_ENABLE);
    bool rodando = true;
    SDL_Event evento;

    while (rodando) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                rodando = false;
            }

			// Atualiza posição
			if(evento.type == SDL_KEYUP) 
				if(evento.key.keysym.sym == SDLK_ESCAPE) rodando = false;
				else if(evento.key.keysym.sym == SDLK_p)
					if(mouse_in) {mouse_in = false; SDL_ShowCursor(SDL_ENABLE);}

			if(evento.type == SDL_MOUSEBUTTONDOWN){
				mouse_in = true;
				SDL_ShowCursor(SDL_DISABLE);
			}
        }

        // Limpa tela
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Controla câmera
		controle_camera(0.2,0.2);
		atualiza_camera_posicao();

        // Desenha cubo
		glPushMatrix();
			glTranslatef(0,-1,0);
			glScalef(10,0.1,10);
        	drawCube();
		glPopMatrix();

        // Atualiza tela
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}