#include "helper.hpp"

#include <array>

#include <iostream>
#include <random>
#include <thread>
#include <vector>

int SCR_WIDTH = 9*32*3 , SCR_HEIGHT = 6*32*3;
GLFWwindow* window = nullptr;
float scale =0.0f;
float lastX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastY = static_cast<float>(SCR_HEIGHT) / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
object* car = nullptr;
Shader* myShader = nullptr;
Shader* screenShader = nullptr;
std::vector<object*> pickups;
std::vector<object*> dangers;
float potionFactor = 1.0f;
double prevPotion;
double prevEvent = 0;
double prevSpeed = 0;
double prevMud=0;
double prevRaketa=0;
bool pecurkaActive=false;
bool mudActive=false;
bool blur=false;
float k=70.0f;
float alfa = 0.0f;
GLuint framebuffer;
GLuint screenTex;
float pause = 0;
double pauseTime;
int rotation = 0;
bool lastKeys[2] = { false };
int mudCnt;
std::vector<glm::vec3> mudPos;

void processInput(GLFWwindow *win)
{
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);
    if (pause == 0) {
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
    }
    if (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS && !lastKeys[0]) {
        if (pause == 1) {
            pause = 0;
            prevEvent  += glfwGetTime() - pauseTime;
            prevRaketa += glfwGetTime() - pauseTime;
            prevPotion += glfwGetTime() - pauseTime;
            prevSpeed  += glfwGetTime() - pauseTime;
            prevMud    += glfwGetTime() - pauseTime;
        }
        else {
            pause = 1;
            pauseTime = glfwGetTime();
        }
        screenShader->Activate();
        screenShader->setFloat("invert", pause);
    }
    if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS && !lastKeys[1]) {
        rotation++;
        rotation = rotation %4;
        screenShader->Activate();
        screenShader->setInt("rotation", rotation);
    }
   /* if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) {
        Shader testShader("Glsls/testVShader.glsl", "Glsls/testFShader.glsl");
        testShader.Activate();
        for (const object* obj : pickups) {
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
        myShader->Activate();
    }*/

    lastKeys[0] = glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS;
    lastKeys[1] = glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS;
}

void framebuffer_size_callback(GLFWwindow* win, const int width,const int height)
{
    glViewport(0, 0, width, height);
}

int windowinit(const int width, const int height, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &screenTex);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex, 0);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    return 0;
}

std::random_device rd;
std::mt19937 mt(rd());
float random(const int a, const int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return static_cast<float>(dist(mt));
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
    verts[i++] = (px + deltaX)/static_cast<float>(SCR_WIDTH)*6 -1.0f;
    verts[i++] = (py + deltaY)/static_cast<float>(SCR_HEIGHT)*6-1.0f;
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
            if (x==0) off = random(4,4);
            else if (x == tilesX - 1) off = random(5,5);
            else if (x == tilesX/2) off = random(6,10);
            else off = random(11,15);
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

glm::vec4 HSVtoRGB(const float h) {
    constexpr float s = 0.8f;
    constexpr float v = 0.6f;
    float r, g, b;

    const float i =h * 6;
    const float f = h * 6 - i;
    constexpr float p = v * (1 - s);
    const float q = v * (1 - f * s);
    const float t = v * (1 - (1 - f) * s);

    switch(static_cast<int>(i) % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
        default: r=0; g=0; b=0; break;
    }

    return {r, g, b,1};
}

std::vector<float> randomSaturatedColor() {
    std::vector<float> res{};
    res.reserve(4);
    for(int i = 0; i < 4; i++) {
        res.push_back(static_cast<float>(i) * 0.25f +random(1,100)/100);
    }
    return res;
}

void handleCollision(const std::string& ime) {
    if (ime == "pecurka") {
        pecurkaActive = true;
        alfa = random(50,100)/100;
        myShader->setFloat("alfa",alfa);
        const std::vector<float> hsl = randomSaturatedColor();
        myShader->setVec4("filterColor1", HSVtoRGB(hsl[0]));
        myShader->setVec4("filterColor2", HSVtoRGB(hsl[1]));
        myShader->setVec4("filterColor3", HSVtoRGB(hsl[2]));
        myShader->setVec4("filterColor4", HSVtoRGB(hsl[3]));
    }
    else if (ime == "potion") {
        prevPotion = glfwGetTime();
        potionFactor = -1.0f;
    }
    else if (ime == "raketa") std::cout << "asd" << std::endl;
    else if (ime == "vinjak") {
        blur = true;
        k = 70.0f;
    }
    else if (ime == "mud") {
        mudActive = true;
        mudCnt = static_cast<int>(random(10, 40));
        for (int i=0;i<mudCnt;i++) {
            mudPos.emplace_back(random(-100,100)/100,random(-100,100)/100,0);
        }
        prevMud = glfwGetTime();
    }
}

void checkCollisions() {
    hitbox box1 = car->hitbox;
    for (auto it = pickups.begin(); it != pickups.end(); ) {
        hitbox box2 = (*it)->hitbox;
        if ((box1.x < box2.x + box2.w && box1.x + box1.w > box2.x) &&
            (box1.y < box2.y + box2.h && box1.y + box1.h > box2.y)) {
                handleCollision((*it)->ime);
                delete *it;
                it = pickups.erase(it);
            } else ++it;
    }
    for (auto it = dangers.begin(); it != dangers.end(); ) {
        hitbox box2 = (*it)->hitbox;
        if ((box1.x < box2.x + box2.w && box1.x + box1.w > box2.x) &&
            (box1.y < box2.y + box2.h && box1.y + box1.h > box2.y)) {
                handleCollision((*it)->ime);
                delete *it;
                it = dangers.erase(it);
            } else ++it;
    }
}

void respawnParticle(particle_t &p) {
    p.x = random(1,2)==1?car->hitbox.x+0.037f:car->hitbox.x+0.037f+0.075f;
    p.y = car->yOff;
    p.dx = (random(0, 100)-50)/1000.0f*1.2f; // (-0.05 0.05)*1.2
    p.dy = -0.1f-random(0, 50)/100.0f;
    p.life = random(0, 50)/100.0f;
}
