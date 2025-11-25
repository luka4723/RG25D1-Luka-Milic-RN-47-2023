#include "object.hpp"
#include <iostream>
#include "glm/ext/matrix_transform.hpp"
#include "helper.hpp"

object::object(const char* id, const float xOff, const float yOff, const float* vertices, GLuint* indices, const GLsizeiptr vertSize, const GLsizeiptr indSize) {
    ime = id;
    this->xOff = xOff;
    this->yOff = yOff;
    this->vao = new VAO;
    this->vao->Bind();
    this->vbo = new VBO(vertices, vertSize);
    this->ebo = new EBO(indices, indSize);
    this->vao->LinkAttrib(*this->vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), nullptr);
    this->vao->LinkAttrib(*this->vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),reinterpret_cast<void*>(3 * sizeof(float)));
    this->vao->Unbind();
    this->vbo->Unbind();
    this->ebo->Unbind();
}

void object::draw(const float x) {
    if (this->ime != "auto") {
        this->yOff-= x;
        this->hitbox.y-=x;
        if (this->yOff < -1.5f) {
            delete this;
            pickups.erase(std::remove(pickups.begin(), pickups.end(), this), pickups.end());

        }
    }
    this->vao->Bind();
    const auto model = glm::translate(glm::mat4(1.0f),glm::vec3(this->xOff, this->yOff, 0.0f));
    myShader->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}

void object::setHitbox(const float x, const float y, const float w, const float h) {
    this->hitbox = {x,y,w,h};
}