# Stamparazzi

# Linha abaixo para compilar o jogo na minha máquina
g++ Stamparazzi.cpp Adesivo.cpp Audio.cpp Colisao.cpp Draw.cpp Entidade.cpp Jogador.cpp Linear.cpp Solido.cpp Textura.cpp -IC:/Users/PCSTUDENT/.vscode/Stamparazzi/include -LC:/Users/PCSTUDENT/.vscode/Stamparazzi/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lopengl32 -lglu32 -lfreeglut -static-libstdc++ -static-libgcc -o Stamparazzi.exe

# Linha abaixo para rodar em qualquer máquina com MinGW, SDL2, SDL_ttf, FreeGLUT instalados no path do sistema
g++ Stamparazzi.cpp Adesivo.cpp Audio.cpp Colisao.cpp Draw.cpp Entidade.cpp Jogador.cpp Linear.cpp Solido.cpp Textura.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lopengl32 -lglu32 -lfreeglut -o Stamparazzi.exe

