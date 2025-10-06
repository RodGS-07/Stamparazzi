#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

// Função para converter SDL_Surface em textura OpenGL
GLuint SDL_SurfaceToTexture(SDL_Surface* surf) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint mode = (surf->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, mode, surf->w, surf->h,
                 0, mode, GL_UNSIGNED_BYTE, surf->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

// Renderiza texto em overlay 2D
void renderText(TTF_Font* font, const char* msg, int x, int y) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surf = TTF_RenderText_Blended(font, msg, white);
    if (!surf) return;

    GLuint tex = SDL_SurfaceToTexture(surf);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 600, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(x, y);
        glTexCoord2f(1,0); glVertex2f(x + surf->w, y);
        glTexCoord2f(1,1); glVertex2f(x + surf->w, y + surf->h);
        glTexCoord2f(0,1); glVertex2f(x, y + surf->h);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDeleteTextures(1, &tex);
    SDL_FreeSurface(surf);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Erro TTF: " << TTF_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL2 + OpenGL + GLUT + TTF",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glEnable(GL_DEPTH_TEST);

    // Carrega fonte
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "Erro carregando fonte" << std::endl;
        return -1;
    }

    bool running = true;
    SDL_Event event;

    float angle = 0.0f;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, 800.0/600.0, 1.0, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0.0,0.0,5.0, 0.0,0.0,0.0, 0.0,1.0,0.0);

        // Desenha esfera GLUT
        glPushMatrix();
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        glColor3f(0.2f, 0.7f, 1.0f);
        glutSolidSphere(1.0, 32, 32);
        glPopMatrix();

        // Desenha texto 2D
        renderText(font, "SDL2 + GLUT + OpenGL + TTF", 10, 10);

        SDL_GL_SwapWindow(window);

        angle += 0.5f;
        SDL_Delay(16); // ~60 FPS
    }

    TTF_CloseFont(font);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
