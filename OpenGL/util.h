#ifndef _UTIL_H__
#define _UTIL_H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct window_attribute {
    char       *win_title;
    int         win_height;
    int         win_width; 
    //...
} win_attr_t;

struct window_ops {
    int (*create)(win_attr_t *attr,);
    void (*destory)(Window *win);
    void (*set_attribute)(win_attr_t * attr);
} ;

typedef struct window {
    win_attr_t        attr;
    struct winodw_ops ops;
} Window;

typedef struct shader_s {
    GLuint  shader; //shader program
    GLchar *shader_code;
    GLuint  shader_code_size;
} shader_t;

typedef struct shader {
    shader_t *(*create)(const GLchar *vertex_path, const GLchar *fragment_pth); 
    void (*destory)(shader_t *shader);
    void (*use)(shader_t *shader);
} Shader;


#endif /* !_UTIL_H__ */
