#include "VBO.hpp"

VBO::VBO(const GLfloat* vertices, GLsizeiptr size)
{
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    glBufferData(GL_ARRAY_BUFFER,size,vertices,GL_STATIC_DRAW);
}

void VBO::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER,ID);
}
void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::Delete()
{
    glDeleteBuffers(1,&ID);
}
void VBO::changeArray(const GLfloat* vertices) {
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

}
