#ifndef INC_2D_GRAFIKA_PROJEKAT_OBJECT_HPP
#define INC_2D_GRAFIKA_PROJEKAT_OBJECT_HPP
#include <string>

#include "EBO.hpp"
#include "VAO.hpp"

struct hitbox {
    float x, y, w, h;
};
class object {
    public:
        std::string ime;
        VAO* vao;
        VBO* vbo;
        EBO* ebo;
        float xOff;
        float yOff;
        hitbox hitbox{};
        object(const char *id, float xOff, float yOff, const float *vertices, GLuint *indices, GLsizeiptr vertSize,GLsizeiptr indSize);

        void setHitbox(float x, float y, float w, float h);

        void draw(float x);
};


#endif