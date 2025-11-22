#include "helper.hpp"
#include "camera.hpp"
#include <iostream>

int SCR_WIDTH = 9*32*3 , SCR_HEIGHT = 6*32*3;
GLFWwindow* window = nullptr;
float scale =0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float carXOffset = -0.2f*aspect;
float carYOffset = -0.8f;


void processInput(GLFWwindow *win)
{
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        if (carYOffset < 0.3) carYOffset+= 0.01f;
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        if (carYOffset > -1.0 )carYOffset-= 0.01f;
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        if (carXOffset > -0.95f + 1.0/13) carXOffset-= 0.01f;
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        if (carXOffset <  0.95f - 1.0/13 - 0.4*aspect)carXOffset+= 0.01f;
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
float random(int a, int b) {
    return static_cast<float>(a + rand() % (b - a + 1));
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
            if (x==0) off = random(2,2);
            else if (x == tilesX - 1) off = random(3,3);
            else if (x == tilesX/2) off = random(4,8);
            else off = random(9,13);
            for (int pos = 0; pos < 6; pos++) coordSetter(vert,px,py,i,pos,off*offset);
        }
    }
}
