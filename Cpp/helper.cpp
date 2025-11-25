#include "helper.hpp"

#include <array>

#include "camera.hpp"
#include <iostream>
#include <random>
#include <thread>
#include <vector>

int SCR_WIDTH = 9*32*3 , SCR_HEIGHT = 6*32*3;
GLFWwindow* window = nullptr;
float scale =0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
object* car = nullptr;
Shader* myShader = nullptr;
std::vector<object*> pickups;
float potionFactor = 1.0f;
double prevPecurka;
double prevPotion;
bool pecurkaActive=false;
glm::vec4 filt1;
glm::vec4 filt2;
glm::vec4 filt3;
glm::vec4 filt4;

void processInput(GLFWwindow *win)
{
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        if ((car->yOff <0.3f && potionFactor == 1.0f) || (car->yOff > -1.0 && potionFactor == -1.0f )) {
            car->yOff+= 0.01f*potionFactor;
            car->hitbox.y += 0.01f*potionFactor;
        }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        if ((car->yOff > -1.0 && potionFactor == 1.0f) || (car->yOff <0.3f && potionFactor == -1.0f )) {
            car->yOff-= 0.01f*potionFactor;
            car->hitbox.y -= 0.01f*potionFactor;
        }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        if ((car->xOff > -0.95f + 1.0/13 && potionFactor==1.0f) || (car->xOff <  0.95f - 1.0/13 - 0.4*aspect && potionFactor == -1.0f)) {
            car->xOff-= 0.01f*aspect*potionFactor;
            car->hitbox.x -= 0.01f*aspect*potionFactor;
        }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        if ((car->xOff <  0.95f - 1.0/13 - 0.4*aspect && potionFactor==1.0f) || (car->xOff > -0.95f + 1.0/13 && potionFactor == -1.0f)) {
            car->xOff+= 0.01f*aspect*potionFactor;
            car->hitbox.x+=0.01f*aspect*potionFactor;
        }
    if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) {
        Shader testShader("Glsls/testVShader.glsl", "Glsls/testFShader.glsl");
        testShader.Activate();
        for (object* obj : pickups) {
            float testVerts[] = {
                obj->hitbox.x, obj->hitbox.y,
                obj->hitbox.x + obj->hitbox.w, obj->hitbox.y,
                obj->hitbox.x + obj->hitbox.w, obj->hitbox.y + obj->hitbox.h,
                obj->hitbox.x, obj->hitbox.y + obj->hitbox.h
            };
            GLuint testIndices[6] = {0, 1, 2, 0, 3, 2};
            VAO testVAO;
            testVAO.Bind();
            VBO testVBO(testVerts, sizeof(testVerts));
            EBO testEBO(testIndices, sizeof(testIndices));
            testVBO.Bind();
            testEBO.Bind();
            testVAO.LinkAttrib(testVBO, 0, 2, GL_FLOAT, 2 * sizeof(float), nullptr);
            testVAO.Unbind();
            testVBO.Unbind();
            testEBO.Unbind();
            testShader.Activate();
            glm::mat4 model = glm::mat4(1.0f);
            testShader.setMat4("model", model);
            testVAO.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            testVAO.Unbind();
        }
    }


}

void framebuffer_size_callback(GLFWwindow* win, const int width,const int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* win, const double xposIn, const double yposIn)
{
    const auto xpos = static_cast<float>(xposIn);
    const auto ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    const float xoffset = xpos - lastX;
    const float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

}

int windowinit(const int width, const int height, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}
float random(const int a, const int b) {
    static std::mt19937 rng(std::random_device{}()); // inicijalizacija generatora
    std::uniform_int_distribution<int> dist(a, b);
    return static_cast<float>(dist(rng));
}
void coordSetter(float* verts, const float px, const float py,int &i, const int pos, const float off) {
    float deltaX;
    float deltaY;

    if (pos ==1) {					 // bot r
        deltaX = tileSize;
        deltaY = 0.0f;
    }
    else if (pos == 2 || pos == 4) { // top r
        deltaX = tileSize;
        deltaY = tileSize;
    }
    else if (pos == 5) {			 // top l
        deltaX = 0.0f;
        deltaY = tileSize;
    }
    else {							 // bot l
        deltaX = 0.0f;
        deltaY = 0.0f;
    }
    verts[i++] = (px + deltaX)/SCR_WIDTH*6 -1.0f;
    verts[i++] = (py + deltaY)/SCR_HEIGHT*6-1.0f;
    verts[i++] = 0.0f;
    verts[i++] = off + (deltaX == 0.0f ? 0.0f : offset);
    verts[i++] = deltaY == 0.0f ? 0.0f : 1.0f;
}

void makeMap(float* vert) {
    int i=0;

    for(int y = 0; y < tilesY; y++) {
        for(int x = 0; x < tilesX; x++) {
            const float px = static_cast<float>(x) * tileSize;
            const float py = static_cast<float>(y) * tileSize;
            float off;
            if (x==0) off = random(3,3);
            else if (x == tilesX - 1) off = random(4,4);
            else if (x == tilesX/2) off = random(5,9);
            else off = random(10,14);
            for (int pos = 0; pos < 6; pos++) coordSetter(vert,px,py,i,pos,off*offset);
        }
    }
}
std::array<float,20> makeArray(const float object_sizex, const float object_sizey, const float texpos) {
    const std::array<float,20> verts = {
        0.0f,				0.0f,		 0.0f, texpos*offset	, 0.0f, // bl
        object_sizex*aspect, 0.0f,		 0.0f, (texpos+1)*offset, 0.0f, // br
        object_sizex*aspect, object_sizey, 0.0f, (texpos+1)*offset, 1.0f, // tr
        0.0f,				object_sizey, 0.0f, texpos*offset	, 1.0f  // tl
    };
    return verts;
}

float getColor() {
    if (random(0, 100) < 50) return random(0, 20) / 100.0f;
    return random(80, 100) / 100.0f;
}

void handleCollision(const std::string& ime) {
    if (ime == "pecurka") {
        pecurkaActive = true;
        prevPecurka = glfwGetTime();
        myShader->setFloat("alfa",0.45f);
        filt1 = glm::vec4(getColor(),getColor(),getColor(),1.0f);
        filt2 = glm::vec4(getColor(),getColor(),getColor(),1.0f);
        filt3 = glm::vec4(getColor(),getColor(),getColor(),1.0f);
        filt4 = glm::vec4(getColor(),getColor(),getColor(),1.0f);
    }
    else if (ime == "potion") {
        prevPotion = glfwGetTime();
        potionFactor = -1.0f;
    }
}

void checkCollisions() {
    hitbox box1 = car->hitbox;
    for (object* obj : pickups) {
        hitbox box2 = obj->hitbox;
        if ((box1.x<box2.x+box2.w && box1.x + box1.w > box2.x)&&
            (box1.y < box2.y + box2.h &&box1.y + box1.h > box2.y)) {
            handleCollision(obj->ime);
            delete obj;
            pickups.erase(std::remove(pickups.begin(), pickups.end(), obj), pickups.end());
            }
    }
}
