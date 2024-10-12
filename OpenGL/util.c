#include "util.h"
#include <stdio.h>

int create_win(win_attr_t *attr, Window *win)
{
    // init GLFW
    glfwInit();
    // set GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    //create window
    GLFWwindow * window = glfwCreateWindow(attr->win_width, attr->win_height, attr->win_title, NULL, NULL);
    if (window == NULL) {
        printf("create window error\n");
        glfwTerminate();
        return -1;
    }
     
    //set window context to current context 
    glfwMakeContextCurrent(window);
    
    win->window = window;
    return 0;
}

int main()
{
    Window  win = { 
        .create = create_win,
    };
    
    win_attr_t win_attr = {
        .win_height = 600,
        .win_width = 800,
        .win_title = "OpenGL test",
    };
    win.create(&win_attr, &win);

    while (!glfwWindowShouldClose(win.window)) {
        // check events
        glfwPollEvents();
        //drawing
      //  changeColor(0.03f);
        GLfloat color[] = {0.2f, 0.3f, 0.5f};
        glClearColor(color[0], color[1], color[2], 1.0f);        
        glClear(GL_COLOR_BUFFER_BIT);        
        //swap buffer ???
        glfwSwapBuffers(win.window);
    }

    glfwTerminate();
    
    return 0;
}
