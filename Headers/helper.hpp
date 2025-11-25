#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.hpp"
#include "object.hpp"
#include "shader.hpp"

extern GLFWwindow* window;

extern int SCR_WIDTH;
extern int SCR_HEIGHT;
extern float lastX;
extern float lastY;
extern bool firstMouse;
extern float deltaTime;
extern float lastFrame;
extern float carXOffset;
extern float carYOffset;
extern constexpr float aspect = 6.0f/9.0f;
extern constexpr int tilesX = 9;
extern constexpr int tilesY = 6;
extern constexpr float tileSize = 32.0f;
extern constexpr float offset = 1.0f/15;
extern Shader* myShader;
extern object* car;
extern std::vector<object*> pickups;
extern float potionFactor;
extern double prevPotion;
extern double prevPecurka;
extern bool pecurkaActive;
extern glm::vec4 filt1;
extern glm::vec4 filt2;
extern glm::vec4 filt3;
extern glm::vec4 filt4;


void framebuffer_size_callback(int width, int height);
void processInput(GLFWwindow *window);
int windowinit(int width, int height, const char* title);
void mouse_callback( double xposIn, double yposIn);
void makeMap(float* vert);
void coordSetter(float* verts, float px,  float py,int &i,  int pos,  float off);
float random(int a, int b);
void handleCollision(const std::string& ime);
float getColor();
std::array<float,20> makeArray( float object_sizex,  float object_sizey,  float texpos);
void checkCollisions();



#endif
