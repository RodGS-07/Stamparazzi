# Stamparazzi

# Linha abaixo para compilar o jogo na minha máquina
g++ stamparazzi.cpp -IC:/Users/PCSTUDENT/.vscode/Stamparazzi/include -LC:/Users/PCSTUDENT/.vscode/Stamparazzi/lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lfreeglut -static-libstdc++ -static-libgcc -o stamparazzi.exe

# Linha abaixo para rodar em qualquer máquina com MinGW, SDL2, FreeGLUT instalados no path do sistema
g++ stamparazzi.cpp -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lfreeglut -o stamparazzi.exe

