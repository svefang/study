/**
 * @brief: gcc window.c -o window -lGL -lglfw -lGLEW
 * */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

GLfloat color[] = {0.6f, 0.3f, 0.4f};

bool colorIncreasing = true;
void changeColor(float deltaTime)
{
    //color[0] = (float)rand() / RAND_MAX;
    //color[1] = (float)rand() / RAND_MAX;
    //color[2] = (float)rand() / RAND_MAX;

    if (colorIncreasing) {
        color[0] += deltaTime;  
        color[1] -= deltaTime;  
        color[2] -= deltaTime;  
        if (color[0] >= 1.0f || color[2] <= 0.0f || color[1]
            <= 0.0f) {
            colorIncreasing = false; 
        }
    } else {
        color[0] -= deltaTime;  
        color[1] += deltaTime;  
        color[2] += deltaTime;  
        if (color[1] <= 0.0f || color[1] >= 1.0f || color[2] 
            >= 1.0f) {
            colorIncreasing = true; 
        }
    
    }
}


/*
 * 着色器语言 CLSL,需要运行时编译
 * in: 声明一个输入变量
 * out:声明一个输出变量
 */
//OpenGL是一个大的状态机，对数据的任何操作，都需要从最开始往后传入
//vec4中有四个属性，前三个分别为x,y,z轴座标，最后一个为透视划分
#if 0 //颜色不变
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

#endif
// 颜色可变 ,必须通过顶点着色器传入
const GLchar *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 color;\n"
    "out vec4 changeColor;\n" //顶点着色器的输出，会被作为片段着色器的输入
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "changeColor = vec4(color, 1.0f);\n"
    "}\0";

const GLchar *fragmentShaderSource = "#version 330 core\n"
    "in vec4 changeColor;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "color = changeColor;\n" 
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

#if 0
GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};
#endif
// 坐标 + 颜色
GLfloat vertices[] = {
    //position
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
    //color
     0.2f, 0.3f, 0.8f, 
};

bool isIncreasing = true;
void changeVertices(float deltaTime)
{
    if (isIncreasing) {
        vertices[0] += deltaTime;  
        vertices[1] += deltaTime;  
        vertices[3] -= deltaTime;  
        vertices[4] += deltaTime;  
        vertices[7] -= deltaTime;  
        if (vertices[1] >= 1.0f || vertices[7] <= -1.0f || vertices[3] <= -1.0f \
            || vertices[0] >= 1.0f || vertices[4] >= 1.0f) {
            isIncreasing = false; 
        }
    } else {
        vertices[0] -= deltaTime;  
        vertices[1] -= deltaTime;  
        vertices[3] += deltaTime;  
        vertices[4] -= deltaTime;  
        vertices[7] += deltaTime;  
        if (vertices[1] <= -1.0f || vertices[7] >= 1.0f || vertices[3] >= 1.0f \
            || vertices[0] <= -1.0f || vertices[4] <= -1.0f) {
            isIncreasing = true; 
        }
    
    }
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
    
    //如果此处设置的宽度与高度比glfwCreateWindow中的小，则后续渲染时只会在这个范围内，剩余的部分则空出来
    int weight, high;
    glfwGetFramebufferSize(window, &weight, &high);
    glViewport(0, 0, weight, high);

    //function prototype    
    //void key_callbakc(GLFWwindow * window, int key, int scancode, int action, int mode)
    glfwSetKeyCallback(window, key_callback);
    
    /**
     * @brief: 
     *      1. 创建着色器
     *      2. 编写顶点着色器源码
     *      3. 编写片段着色器源码
     *      4. 将着色器源码附着到着色器上
     *      5. 编译着色器
     *      6. 创建着色器对象，并将生成的顶点着色器与片段着色器附着到对象上
     *      7. 链接生成着色器程序对象
     *      8. 告诉OpenGL如何解释缓冲中的数据
     *      
     * */


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
    /*
     * VBO: vertex buffer object: 顶点缓冲对象
     * 顶点着色器会在GPU上创建内存用于存储顶点数据，同时还要告诉OpenGL如何解释内存中的数据,并指定其如何发送给显卡,而
     * 顶点缓冲对象就是用来管理这这篇内存的
     * VAO: vertex array object: 顶点数组对象
     * 当有多个顶点缓冲对象时，使用VAO来存储
     * */
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    //GLAPI void APIENTRY glGenVertexArrays (GLsizei n, GLuint *arrays);
    glBindVertexArray(VAO);

    //buffer type: GL_ARRAY_BUFFER, 
    //指定冲对象的类型
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点数据复制到缓冲的内存中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /**
     * 告诉OpenGL如何解析顶点着色数据
     * 内存格式：
     * 
     *  Byte   0   4   8  12  16  20  24  28  32  36  40  44  48
     *         |-----------|-----------|-----------|-----------|
     *         | x | y | z | x | y | z | x | y | z | R | G | B |
     *  position                                   color
     */
    /*
     *   作用：定义顶点属性的布局，告诉OpenGL如何解释缓冲中的顶点数据
     *   GLAPI void APIENTRY glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
     *      @index: 指定顶点的属性，与VertexShaderSource中layout中 location对应      
     *      @size: 属性的大小
     *      @type: 顶点数据的类型
     *      @normalized:是否希望数据被标准化，如何设置为GL_TRUE,则所有的数据会被映射到0 - 1, （signed的类型为 -1 - 1）
     *      @stride:数据的步长，告诉OpenGL顶点数据属性组的间隔
     *      @pointer:数据在缓冲中的偏移
     *
     * */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    //启动顶点属性，默认是禁止的
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) (9 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        // check events
        glfwPollEvents();
        //drawing
        //glClearColor(color[0], color[1], color[2], 1.0f);        
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);        
        glClear(GL_COLOR_BUFFER_BIT);        
        //激活此对象 
        glUseProgram(shaderProgram);
    
        glBindVertexArray(VAO);        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);        
    
        #if 0 //change vertex position
        changeVertices(0.04);
        #endif
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        /*
         * GLAPI void APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
         *  该函数允许更新缓冲对象中的部分数据
         *  @target:指定更新数据的缓冲对象目标， 常见的有：GL_ARRAY_BUFFER(顶点属性), GL_ELEMENT_ARRAY_BUFFER(索引缓冲区), GL_UNIFORM_BUFFER(统一变量缓冲区)
         */
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        //changeColor(0.01);
        //glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9, 3 * sizeof(GLfloat), color);

        //swap buffer ???
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
