#include "object.hpp"
#include <iostream>
#include "glm/ext/matrix_transform.hpp"
#include "helper.hpp"

object::object(const char* id, const float xOff, const float yOff, const float* vertices, const GLuint* indices, const GLsizeiptr vertSize, const GLsizeiptr indSize,const float fall) {
    ime = id;
    this->xOff = xOff;
    this->yOff = yOff;
    this->vao = new VAO;
    this->vao->Bind();
    this->vbo = new VBO(vertices, vertSize);
    this->ebo = new EBO(indices, indSize);
    VAO::LinkAttrib(*this->vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), nullptr);
    VAO::LinkAttrib(*this->vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),reinterpret_cast<void*>(3 * sizeof(float)));
    VAO::Unbind();
    VBO::Unbind();
    EBO::Unbind();
    this->fall = fall;
}
void object::draw() {
    if (this->ime != "auto") {
        this->yOff -= fall;
        this->hitbox.y -= fall;
        if (this->yOff < -1.5f) {
            if (this->ime == "raketa" || this->ime == "mud") dangers.erase(std::remove(dangers.begin(), dangers.end(), this), dangers.end());
            else pickups.erase(std::remove(pickups.begin(), pickups.end(), this), pickups.end());
            delete this;
            return;
        }
    }
    this->vao->Bind();
    const auto model = glm::translate(glm::mat4(1.0f), glm::vec3(this->xOff, this->yOff, 0.0f));
    myShader->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void object::setHitbox(const float x, const float y, const float w, const float h) {
    this->hitbox = {x,y,w,h};
}