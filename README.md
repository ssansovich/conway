Conway's game of life implemented in C with visualization using OpenGL via GLFW+GLAD.
Just some practice using C and OpenGL.

Used the gen1 GLAD web generator. The gen2 one doesn't work with the learnopengl.com code.

Run with on MacOS:
gcc -o output game.c glad.c -framework Cocoa -framework OpenGL -Llib -lglfw
