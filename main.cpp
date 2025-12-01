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
	Shader blurShader("Glsls/blurVShader.glsl", "Glsls/blurFShader.glsl");
	Shader smokeShader("Glsls/smokeVShader.glsl", "Glsls/smokeFShader.glsl");

	float quadVerts[] ={
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
	};

	VAO quadVAO;
	quadVAO.Bind();
	VBO quadVBO(quadVerts,sizeof(quadVerts));
	quadVAO.LinkAttrib(quadVBO,0,2,GL_FLOAT,4*sizeof(float),nullptr);
	quadVAO.LinkAttrib(quadVBO,1,2,GL_FLOAT,4*sizeof(float),reinterpret_cast<void *>(2 * sizeof(float)));
	quadVAO.Unbind();

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
	car = new object("auto",-0.2f*aspect,-0.8f,verts.data(),carIndices,sizeof(float)*verts.size(),sizeof(carIndices),0);
	car->setHitbox(car->xOff+0.06f,car->yOff,0.22f*aspect,0.405f);

	for (auto & particle : particles) respawnParticle(particle);
	VAO dots;
	dots.Bind();
	VBO dotVBO(particles,sizeof(particles));
	dots.LinkAttrib(dotVBO,0,2,GL_FLOAT,sizeof(particle_t),nullptr);
	dots.Unbind();
	smokeShader.setVec2("cent",glm::vec2((car->hitbox.x+0.037f+car->hitbox.x+0.037f+0.075f)/2,car->yOff));
	smokeShader.setVec3("col",0.1,0.1,0.1);

    auto map_tileset = Texture("resources/test2.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,0);
	map_tileset.Bind();
	map_tileset.texUnit(*myShader, "sheet");

	double prevEvent = glfwGetTime();
	double spawnInterval = random(3,3);
	double rocketInterval = random(3,10);

	double map1Loc=0,map2Loc=2;

	blurShader.Activate();
	blurShader.setInt("screenTexture", 0);


	float w =0.0f,h=0.0f;
	while (!glfwWindowShouldClose(window))
	{


		const auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		const double crntTime = glfwGetTime();
		smokeShader.Activate();
		smokeShader.setVec2("cent",glm::vec2((car->hitbox.x+0.037f+car->hitbox.x+0.037f+0.075f)/2,car->yOff));
		myShader->Activate();
		if (crntTime - prevEvent >= spawnInterval)
		{
			object_sizex = 0.27f;
			object_sizey = 0.27f;
			texpos=random(1,3);
			const char* ime = nullptr;
			if (texpos == 1) ime = "pecurka";
			else if (texpos == 2) ime = "potion";
			else if (texpos == 3) ime = "nasilje";
			verts = makeArray(object_sizex, object_sizey, texpos);
			auto* obj = new object(ime, -0.83f+random(0,148)/100,1.0f, verts.data(), carIndices, sizeof(float)*verts.size(),sizeof(carIndices),0.2f*deltaTime);
			float hb_x = obj->xOff+0.03f;
			float hb_y = obj->yOff+0.01f;
			float hb_w = 0.19f*aspect;
			float hb_h = 0.2f;
			if (texpos == 3) {
				hb_x = obj->xOff+0.06f;
				hb_y = obj->yOff+0.01f;
				hb_w = 0.1f*aspect;
				hb_h = 0.2f;
			}
			obj->setHitbox(hb_x,hb_y,hb_w,hb_h);
			pickups.push_back(obj);
			spawnInterval = random(3,3);
			prevEvent = crntTime;
		}
		/*if (crntTime - prevRaketa >= rocketInterval) {
			float cnt = random(3,7);
			for (int i = 0;i<cnt;i++) {
				object_sizex = 0.35f;
				object_sizey = 0.35f;
				//texpos=random(1,2);
				const char* ime = nullptr;
				verts = makeArray(object_sizex, object_sizey, random(15,15));
				auto* obj = new object("raketa", -0.9+random(1,157)/100,1.0f, verts.data(), carIndices, sizeof(float)*verts.size(),sizeof(carIndices),0.5f*deltaTime);
				obj->setHitbox(obj->xOff+0.095f,obj->yOff,0.07f*aspect,0.23f);
				dangers.push_back(obj);
			}
			rocketInterval = random(3,10);
			prevRaketa = crntTime;
		}*/
		if (alfa < 0) {
			pecurkaActive = false;
			prevPecurka = crntTime;
			myShader->setFloat("alfa",0.0f);
			smokeShader.Activate();
			smokeShader.setVec3("col",0.1,0.1,0.1);
			myShader->Activate();
		}
		if  (crntTime - prevPotion >= 5.0f) potionFactor = 1.0f;

		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT);

		myShader->Activate();
		map_tileset.Bind();

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
			map1.changeArray(vertices,sizeof(vertices)/sizeof(float));
			map1Loc += 4.0f;
		}
		if (map2Loc < -2) {
			makeMap(vertices2);
			map2.changeArray(vertices2,sizeof(vertices)/sizeof(float));
			map2Loc += 4.0f;
		}

		car->draw();
		for (object* obj : pickups) obj->draw();
		//for (object* obj : dangers) obj->draw();
		checkCollisions();

		for (auto & p : particles) {
			p.life -= deltaTime;
			if (p.life > 0.0f) {
				p.x += p.dx * deltaTime;
				p.y += p.dy * deltaTime;
			} else respawnParticle(p);
		}

		smokeShader.Activate();
		if (pecurkaActive) {
			smokeShader.setVec3("col",
				glm::sin(static_cast<float>(crntTime)*2.0f) * 0.5f + 0.5f,
				glm::sin(static_cast<float>(crntTime)*2.0f+ 2.094f) * 0.5f + 0.5f,
				glm::sin(static_cast<float>(crntTime)*2.0f+ 4.188f) * 0.5f + 0.5f);
		}
		else smokeShader.setVec3("col",0.1,0.1,0.1);
		dots.Bind();
		dotVBO.changeArray(particles,MAX_PARTICLES * sizeof(particle_t));
		glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
		myShader->Activate();

		if (pecurkaActive) {
			const float t = static_cast<float>(crntTime) * 1.5f;

			/*filt1.r += (glm::sin(random(1,5)*t)) * 0.005f;
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
			filt4 = glm::clamp(filt4, glm::vec4(0.0f), glm::vec4(1.0f));*/

			myShader->setVec4("filterColor1", filt1);
			myShader->setVec4("filterColor2", filt2);
			myShader->setVec4("filterColor3", filt3);
			myShader->setVec4("filterColor4", filt4);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (blur) {
			const auto r = static_cast<float>(1.5f * glfwGetTime());
			h = cos(r);
			w = sin(r);
			k-=10.0f*deltaTime;
			if (k<0) blur = false;
		}
		blurShader.Activate();
		blurShader.setVec2("vec",w*k,h*k);
		quadVAO.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	VAO1.Delete();
    map1.Delete();
    myShader->Delete();
    map_tileset.Delete();

    return 0;
}

