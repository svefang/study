/**
 * @brief: gcc window.c -o window -lGL -lglfw -lGLEW
 * */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

GLfloat color[] = {0.2f, 0.3f, 0.3f};


//着色器语言 CLSL,需要运行时编译
//vec4中有四个属性，前三个分别为x,y,z轴座标，最后一个为透视划分
const GLchar *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";

const GLchar *fragmentShaderSource = "#version 330 core\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" 
    "}\n\0";



bool isResize = false;
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    //if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    //    if (!isResize) {
    //        glfwSetWindowSize(window, 300, 300);
    //        isResize = true;
    //    } else {
    //        glfwSetWindowSize(window, 800, 600);
    //        isResize = false;
    //    }
    //}
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};


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
    
    //如果此处设置的宽度与高度比glfwCreateWindow中的小，则后续渲染时只会在这个范围内，剩余的部分则空出来
    int weight, high;
    glfwGetFramebufferSize(window, &weight, &high);
    glViewport(0, 0, weight, high);

    //function prototype    
    //void key_callbakc(GLFWwindow * window, int key, int scancode, int action, int mode)
    glfwSetKeyCallback(window, key_callback);


    //创建着色器
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    //将着色器源码附着在着色器上
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //检查着色器源码编译结果    
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 128, NULL, infoLog);
        printf("ERROR: COMPILE VERTEXSHADER FAILED\n");
        return -1;
    }

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();

    //将两个着色器附加到对象上
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //链接
    glLinkProgram(shaderProgram);

    //检查链接是否失败
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR: LINK SHADERPROGRAM　FAILED\n");
        return -1;
    }
    

    //因为shaderProgram已经生成，不需要顶点着色器对象和片段着色器对象，直接删除
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    //create OpenGL vbo    
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    //buffer type: GL_ARRAY_BUFFER, 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //告诉OpenGL如何解析顶点着色数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        // check events
        glfwPollEvents();
        //drawing
        glClearColor(color[0], color[1], color[2], 1.0f);        
        glClear(GL_COLOR_BUFFER_BIT);        
        //激活此对象 
        glUseProgram(shaderProgram);
    
        glBindVertexArray(VAO);        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);        
    

        //swap buffer ???
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
