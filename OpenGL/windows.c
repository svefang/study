/**
 * @brief: gcc window.c -o window -lGL -lglfw -lGLEW
 * */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

bool isIncreasing = true;

float color[] = {1.2f, 1.2f, 1.2f};

void changeColor(float deltaTime)
{
    if (isIncreasing) {
        color[0] -= deltaTime;  
        color[1] += deltaTime;  
        color[2] -= deltaTime;  
        if (color[0] <= 0.0f || color[2] <= 0.0f) {
            isIncreasing = false; 
        }
    } else {
        color[0] += deltaTime;  
        color[1] -= deltaTime;  
        color[2] += deltaTime;  
        if (color[1] <= 0.0f) {
            isIncreasing = true; 
        }
    
    }
}

bool isResize = false;
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (!isResize) {
            glfwSetWindowSize(window, 300, 300);
            isResize = true;
        } else {
            glfwSetWindowSize(window, 800, 600);
            isResize = false;
        }
    }
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


int main()
{
    // init GLFW
    glfwInit();
    // set GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    //create window
    GLFWwindow * window = glfwCreateWindow(800, 600, "OpenGL test", NULL, NULL);
    if (window == NULL) {
        printf("create window error\n");
        glfwTerminate();
        return -1;
    }
   
    //set window context to current context 
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("glewInit error\n");
        return -1; 
    }
    
    int weight, high;
    //如果此处设置的宽度与高度比glfwCreateWindow中的小，则后续渲染时只会在这个范围内，剩余的部分则空出来
    glfwGetFramebufferSize(window, &weight, &high);
    glViewport(0, 0, weight, high);
    //function prototype    
    //void key_callbakc(GLFWwindow * window, int key, int scancode, int action, int mode)
    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window)) {
        // check events
        glfwPollEvents();
        //drawing
        changeColor(0.03f);
        glClearColor(color[0], color[1], color[2], 1.0f);        
        glClear(GL_COLOR_BUFFER_BIT);        
        //swap buffer ???
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
