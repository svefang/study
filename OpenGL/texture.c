/**
 * @brief:  gcc texture.c -o texture  -lGL -lglfw -lGLEW  -lSOIL -lm
 * */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

GLfloat color[] = {0.2f, 0.3f, 0.3f};

const GLchar *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 color;\n"
    "layout (location = 2) in vec2 texcolor;\n"
    "out vec4 changeColor;\n" //顶点着色器的输出，会被作为片段着色器的输入
    "out vec2 texColor;\n" 
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position, 1.0f);\n"
    "texColor = texcolor;\n"
    "}\0";

const GLchar *fragmentShaderSource = "#version 330 core\n"
    "in vec4 changeColor;\n"
    "in vec2 texColor;\n"

    "out vec4 color;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "color = texture(ourTexture, texColor);\n" 
    "}\n\0";

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLfloat vertices[] = {
    //position            // color            //texture
    -0.8f, -0.8f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.8f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     0.8f,  0.8f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    -0.8f,  0.8f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    -0.8f, -0.8f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
};

GLuint indexes[] = {
    0, 1 ,2 ,3, 0
};

/**
 * 纹理坐标的范围是(0,0) 到 (1,1)，如果超过此范围，则会有以下几种情况
 *  1. GL_REPEAT : 重复纹理           |
 *  2. GL_MIRRORED_REPEAT: 镜像纹理   |  可通过glTexParameteri(GL_TERTURE_2D, GL_TEXTURE_WRAP_S, GL_MINRRORED_REPEAT)
                                      |   和glTexParameteri(GL_TERTURE_2D, GL_TEXTURE_WRAP_T, GL_MINRRORED_REPEAT)进行设置
 *  3. GL_GLAMP_TO_EDGE: 边缘拉伸     |
 *  4. GL_CLAMP_TO_BORDER: 超出部分为用户指定的颜色 使用glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color[]);
 * */
//纹理颜色
GLfloat texColor[] = {
    0.0f, 0.0f,
    0.0f, 0.8f,
    0.8f, 0.8f,
    0.8f ,0.0f
};




int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s picture path\n", argv[0]); 
        return -1;
    }
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
    glfwGetFramebufferSize(window, &weight, &high);
    glViewport(0, 0, weight, high);

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

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glGenVertexArrays(1, &EBO);
    glGenBuffers(1, &EBO); 

    glBindVertexArray(VAO);

    //buffer type: GL_ARRAY_BUFFER, 
    //指定冲对象的类型
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点数据复制到缓冲的内存中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ===============================
    //生成纹理
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int image_width, image_height;
    assert(argv[1]);
    unsigned char *image = SOIL_load_image(argv[1], &image_width, &image_height, 0, SOIL_LOAD_RGB);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);//解绑

    // ===============================

    while (!glfwWindowShouldClose(window)) {
        // check events
        glfwPollEvents();
        //drawing
        glClearColor(color[0], color[1], color[2], 1.0f);        
        glClear(GL_COLOR_BUFFER_BIT);        

        glBindTexture(GL_TEXTURE_2D, texture);
        //激活此对象 
        glUseProgram(shaderProgram);
    
        glBindVertexArray(VAO);        
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
        //glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);        

        //glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glfwSwapBuffers(window);
    }
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}
