#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform int isAlive;\n"
    "void main() {\n"
    "   if (isAlive == 1) {\n"
    "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "   } else {\n"
    "       FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
    "   }\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

#define HEIGHT 100
#define WIDTH 100
int GAME_SIZE = HEIGHT * WIDTH;

void createCellVAO(int x, int y, unsigned int *VAO, unsigned int *VBO, unsigned int *EBO) {
    float z = 0.0f;

    float x_start = ((float)x / ((float)WIDTH / 2.0) - 1.0);
    float y_start = ((float)y / ((float)HEIGHT / 2.0) - 1.0);
    float offset = (2.0 / HEIGHT);
    float x_end = x_start + offset;
    float y_end = y_start + offset;

    float vertices[] = {
        x_end,  y_end, z,  // top right
        x_end,  y_start, z,  // bottom right
        x_start,  y_start, z,  // bottom left
        x_start,  y_end, z   // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // 1. bind Vertex Array Object
    glBindVertexArray(*VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return;
}

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }    

    /* === Begin shader setup === */

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s", infoLog);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s", infoLog);
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /* === End shader setup === */

    /* Create game state */

    unsigned int VAOs[GAME_SIZE];
    unsigned int VBOs[GAME_SIZE];
    unsigned int EBOs[GAME_SIZE];
    glGenVertexArrays(GAME_SIZE, VAOs);
    glGenBuffers(GAME_SIZE, VBOs);
    glGenBuffers(GAME_SIZE, EBOs);

    int output[HEIGHT][WIDTH];
    int state[HEIGHT][WIDTH];
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            output[x][y] = 0;
            state[x][y] = rand() % 2;

            int i = (x * WIDTH) + y;
            createCellVAO(x, y, &VAOs[i], &VBOs[i], &EBOs[i]);
        }
    }

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        /* === Input === */
        processInput(window);

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* === Game Logic === */

        // Save the state
        for (int x = 1; x < WIDTH - 1; x++) {
            for (int y = 1; y < HEIGHT - 1; y++) {
                output[x][y] = state[x][y];
            }
        }

        // Run the game of life algo
        for (int x = 1; x < WIDTH - 1; x++) {
            for (int y = 1; y < HEIGHT - 1; y++) {
                int num_neighbors = output[x - 1][y + 1] + output[x + 0][y + 1] + output[x + 1][y + 1] +
                                    output[x - 1][y + 0] + 0                    + output[x + 1][y + 0] +
                                    output[x - 1][y - 1] + output[x + 0][y - 1] + output[x + 1][y - 1];

                if (output[x][y] == 1) {
                    state[x][y] = (num_neighbors == 2) || (num_neighbors == 3);
                } else {
                    state[x][y] = num_neighbors == 3;
                }
            }
        }

        /* === Render === */
        for (int i = 0; i < GAME_SIZE; i++) {
            int x = i / (int)WIDTH;
            int y = i % (int)HEIGHT;

            int vertexColorLocation = glGetUniformLocation(shaderProgram, "isAlive");
            glUniform1i(vertexColorLocation, output[x][y]);
            glUseProgram(shaderProgram);

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
