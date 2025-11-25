#include <algorithm>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ctime>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "helper.hpp"
#include "stb_image.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "object.hpp"



int main() {
	srand(time(nullptr));
	if (windowinit(SCR_WIDTH, SCR_HEIGHT, "Moj OpenGL Prozor") != 0) return -1;

	float vertices[6*tilesX*tilesY*5];
	float vertices2[6*tilesX*tilesY*5];
	float object_sizex = 0.4f;
	float object_sizey = 0.4f;
	float texpos = 0;
	GLuint carIndices[6] = {0, 1, 2, 0, 3, 2};
	myShader = new Shader("Glsls/vShader.glsl", "Glsls/fShader.glsl");

	makeMap(vertices);
	makeMap(vertices2);

    VAO VAO1,VAO2;
    VAO1.Bind();

    VBO map1(vertices, sizeof(vertices));
    VAO1.LinkAttrib(map1,0,3,GL_FLOAT,5*sizeof(float),nullptr);
    VAO1.LinkAttrib(map1,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));
	map1.Unbind();

	VAO2.Bind();
	VBO map2(vertices2, sizeof(vertices2));
	VAO2.LinkAttrib(map2,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	VAO2.LinkAttrib(map2,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));
	map2.Unbind();

	std::array<float, 20> verts = makeArray(object_sizex, object_sizey, texpos);
	car = new object("auto",-0.2f*aspect,-0.8f,verts.data(),carIndices,sizeof(float)*verts.size(),sizeof(carIndices));
	car->setHitbox(car->xOff+0.06f,car->yOff,0.22f*aspect,0.405f);

    auto map_tileset = Texture("resources/test2.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,0);
	map_tileset.Bind();
	map_tileset.texUnit(*myShader, "sheet");

	double prevEvent = glfwGetTime();
	double spawnInterval = random(3,3);

	double map1Loc=0,map2Loc=2;
	glm::vec4 pecurkaOverlay = glm::vec4(0.0f,0.0f,0.0f,1.0f);

	while (!glfwWindowShouldClose(window))
	{
		const auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		const double crntTime = glfwGetTime();

		if (crntTime - prevEvent >= spawnInterval)
		{
			object_sizex = 0.27f;
			object_sizey = 0.27f;
			texpos=random(1,2);
			const char* ime = nullptr;
			if (texpos == 1) ime = "pecurka";
			else if (texpos == 2) ime = "potion";
			verts = makeArray(object_sizex, object_sizey, texpos);
			auto* obj = new object(ime, -0.83f+random(0,148)/100,1.0f, verts.data(), carIndices, sizeof(float)*verts.size(),sizeof(carIndices));
			obj->setHitbox(obj->xOff+0.03f,obj->yOff+0.01f,0.19f*aspect,0.2f);
			pickups.push_back(obj);
			spawnInterval = random(3,3);
			prevEvent = crntTime;
		}
		if (crntTime - prevPecurka >= 5.0f) {
			pecurkaActive = false;
			prevPecurka = crntTime;
			myShader->setFloat("alfa",0.0f);
		}
		if  (crntTime - prevPotion >= 5.0f) potionFactor = 1.0f;

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		myShader->Activate();

		map1Loc -= 0.2f*deltaTime;
		map2Loc -= 0.2f*deltaTime;

		VAO1.Bind();
		auto mapModel = glm::translate( glm::mat4(1.0f),glm::vec3(0.0f, map1Loc, 0.0f));
		myShader->setMat4("model", mapModel);
		glDrawArrays(GL_TRIANGLES, 0, 6*tilesX*tilesY);

		VAO2.Bind();
		mapModel = glm::translate( glm::mat4(1.0f),glm::vec3(0.0f, map2Loc, 0.0f));
		myShader->setMat4("model", mapModel);
		glDrawArrays(GL_TRIANGLES, 0, 6*tilesX*tilesY);

		if (map1Loc < -2) {
			makeMap(vertices);
			map1.changeArray(vertices);
			map1Loc += 4.0f;
		}
		if (map2Loc < -2) {
			makeMap(vertices2);
			map2.changeArray(vertices2);
			map2Loc += 4.0f;
		}

		car->draw(1);
		for (object* obj : pickups) obj->draw(0.2f*deltaTime);
		checkCollisions();

		if (pecurkaActive) {

			const float t = static_cast<float>(crntTime) * 1.5f;

			filt1.r += (glm::sin(random(1,5)*t)) * 0.005f;
			filt1.g += (glm::sin(random(1,5)*t)) * 0.005f;
			filt1.b += (glm::sin(random(1,5)*t)) * 0.005f;

			filt2.r += (glm::sin(random(1,5)*t+3.14f/4) ) * 0.005f;
			filt2.g += (glm::sin(random(1,5)*t+3.14f/4) ) * 0.005f;
			filt2.b += (glm::sin(random(1,5)*t+3.14f/4) ) * 0.005f;

			filt3.r += (glm::sin(random(1,5)*t+3.14f/2) ) * 0.005f;
			filt3.g += (glm::sin(random(1,5)*t+3.14f/2) ) * 0.005f;
			filt3.b += (glm::sin(random(1,5)*t+3.14f/2)) * 0.005f;

			filt4.r += (glm::sin(random(1,5)*t+3.14f/4*3) ) * 0.005f;
			filt4.g += (glm::sin(random(1,5)*t+3.14f/4*3) ) * 0.005f;
			filt4.b += (glm::sin(random(1,5)*t+3.14f/4*3)) * 0.005f;

			filt1 = glm::clamp(filt1, glm::vec4(0.0f), glm::vec4(1.0f));
			filt2 = glm::clamp(filt2, glm::vec4(0.0f), glm::vec4(1.0f));
			filt3 = glm::clamp(filt3, glm::vec4(0.0f), glm::vec4(1.0f));
			filt4 = glm::clamp(filt4, glm::vec4(0.0f), glm::vec4(1.0f));

			myShader->setVec4("filterColor1", filt1);
			myShader->setVec4("filterColor2", filt2);
			myShader->setVec4("filterColor3", filt3);
			myShader->setVec4("filterColor4", filt4);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	VAO1.Delete();
    map1.Delete();
    myShader->Delete();
    map_tileset.Delete();

    return 0;
}

