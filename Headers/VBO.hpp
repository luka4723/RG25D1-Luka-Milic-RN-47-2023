#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>

class VBO
{
    public:
        GLuint ID;
        VBO(const GLfloat* vertices, GLsizeiptr size);

        void Bind() const;
        void Unbind();
        void Delete();

        void changeArray(const GLfloat *vertices);
};

#endif
