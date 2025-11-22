#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
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


void drawObstacle() {

}

int main() {
	if (windowinit(SCR_WIDTH, SCR_HEIGHT, "Moj OpenGL Prozor") != 0) return -1;

	float vertices[6*tilesX*tilesY*5];
	float vertices2[6*tilesX*tilesY*5];
	float carVerts[] = {
		0.0f,		 0.0f , 0.0f, 0.0f, 0.0f, // bl
		0.4f*aspect, 0.0f, 0.0f, offset, 0.0f, // br
		0.4f*aspect, 0.4f, 0.0f, offset, 1.0f, // tr
		0.0f,		 0.4f , 0.0f, 0.0f, 1.0f  // tl
	};
	GLuint carIndices[6] = {0, 1, 2, 0, 3, 2};
	makeMap(vertices);
	makeMap(vertices2);


    Shader myShader("Glsls/vShader.glsl", "Glsls/fShader.glsl");

    VAO VAO1,VAO2, carVAO;
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

	carVAO.Bind();
	VBO car(carVerts,sizeof(carVerts));
	EBO carEBO(carIndices, sizeof(carIndices));
	carVAO.LinkAttrib(car,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	carVAO.LinkAttrib(car,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));
	carVAO.Unbind();
	car.Unbind();
	carEBO.Unbind();

    auto map_tileset = Texture("resources/test2.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,0);
	map_tileset.Bind();
	map_tileset.texUnit(myShader, "sheet");

    float rotation = 0.0f;
	double prevTime = glfwGetTime();

	double map1Loc=0,map2Loc=2;
	while (!glfwWindowShouldClose(window))
	{
		const auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		const double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1.0 / 60)
		{
			rotation -= 1.0f;
			map1Loc -= deltaTime*0.2f;
			map2Loc -= deltaTime*0.2f;
			prevTime = crntTime;
		}

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.Activate();

		VAO1.Bind();
		auto mapModel = glm::translate( glm::mat4(1.0f),glm::vec3(0.0f, map1Loc, 0.0f));
		myShader.setMat4("model", mapModel);
		glDrawArrays(GL_TRIANGLES, 0, 6*tilesX*tilesY);

		VAO2.Bind();
		mapModel = glm::translate( glm::mat4(1.0f),glm::vec3(0.0f, map2Loc, 0.0f));
		myShader.setMat4("model", mapModel);
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
		auto model = glm::translate( glm::mat4(1.0f),glm::vec3(carXOffset, carYOffset, 0.0f));
		carVAO.Bind();
		myShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	VAO1.Delete();
    map1.Delete();
    myShader.Delete();
    map_tileset.Delete();

    return 0;
}

