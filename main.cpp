#include <algorithm>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "helper.hpp"
#include "stb_image.hpp"
#include "texture.hpp"
#include "object.hpp"

int main() {
	if (windowinit(SCR_WIDTH, SCR_HEIGHT, "2d-domaci") != 0) return -1;

	float vertices[6*tilesX*tilesY*5];
	float vertices2[6*tilesX*tilesY*5];
	float object_sizex = 0.4f;
	float object_sizey = 0.4f;
	float texpos = 0;
	const GLuint carIndices[6] = {0, 1, 2, 0, 3, 2};
	myShader = new Shader("Glsls/vShader.glsl", "Glsls/fShader.glsl");
	screenShader = new Shader("Glsls/blurVShader.glsl", "Glsls/blurFShader.glsl");
	const Shader smokeShader("Glsls/smokeVShader.glsl", "Glsls/smokeFShader.glsl");
	const Shader stencilShader("Glsls/stencilVShader.glsl", "Glsls/stencilFShader.glsl");

	float screenVerts[] ={
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
	};
	float circleVerts[] = {
		0.0f * aspect, 0.0f,
		0.070000f * aspect,  0.000000f,
		0.067620f * aspect,  0.014634f,
		0.060622f * aspect,  0.027300f,
		0.049498f * aspect,  0.037871f,
		0.035000f * aspect,  0.046200f,
		0.018274f * aspect,  0.052500f,
		0.000000f * aspect,  0.056000f,
	   -0.018274f * aspect,  0.052500f,
	   -0.035000f * aspect,  0.046200f,
	   -0.049498f * aspect,  0.037871f,
	   -0.060622f * aspect,  0.027300f,
	   -0.067620f * aspect,  0.014634f,
	   -0.070000f * aspect,  0.000000f,
	   -0.067620f * aspect, -0.014634f,
	   -0.060622f * aspect, -0.027300f,
	   -0.049498f * aspect, -0.037871f,
	   -0.035000f * aspect, -0.046200f,
	   -0.018274f * aspect, -0.052500f,
		0.000000f * aspect, -0.056000f,
		0.018274f * aspect, -0.052500f,
		0.035000f * aspect, -0.046200f,
		0.049498f * aspect, -0.037871f,
		0.060622f * aspect, -0.027300f,
		0.067620f * aspect, -0.014634f,
		0.070000f * aspect,  0.000000f,
		0.067620f * aspect,  0.014634f,
		0.060622f * aspect,  0.027300f,
		0.049498f * aspect,  0.037871f,
		0.035000f * aspect,  0.046200f,
		0.018274f * aspect,  0.052500f,
		0.070000f * aspect,  0.000000f
	};
	const VAO screennVAO;
	screennVAO.Bind();
	const VBO quadVBO(screenVerts,sizeof(screenVerts));
	VAO::LinkAttrib(quadVBO,0,2,GL_FLOAT,4*sizeof(float),nullptr);
	VAO::LinkAttrib(quadVBO,1,2,GL_FLOAT,4*sizeof(float),reinterpret_cast<void *>(2 * sizeof(float)));
	VAO::Unbind();

	const VAO stencilVAO;
	stencilVAO.Bind();
	const VBO stencilVBO(circleVerts,sizeof(circleVerts));
	VAO::LinkAttrib(stencilVBO,0,2,GL_FLOAT,2*sizeof(float),nullptr);
	VAO::Unbind();

	makeMap(vertices);
	makeMap(vertices2);

	const VAO VAO1;
    const VAO VAO2;

    VAO1.Bind();
    const VBO map1(vertices, sizeof(vertices));
    VAO::LinkAttrib(map1,0,3,GL_FLOAT,5*sizeof(float),nullptr);
    VAO::LinkAttrib(map1,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));
	VBO::Unbind();

	VAO2.Bind();
	const VBO map2(vertices2, sizeof(vertices2));
	VAO::LinkAttrib(map2,0,3,GL_FLOAT,5*sizeof(float),nullptr);
	VAO::LinkAttrib(map2,1,2,GL_FLOAT,5*sizeof(float),reinterpret_cast<void *>(3 * sizeof(float)));
	VBO::Unbind();

	std::array<float, 20> verts = makeArray(object_sizex, object_sizey, texpos);
	car = new object("auto",-0.2f*aspect,-0.8f,verts.data(),carIndices,sizeof(float)*verts.size(),sizeof(carIndices),0);
	car->setHitbox(car->xOff+0.06f,car->yOff,0.22f*aspect,0.405f);

	for (auto & particle : particles) respawnParticle(particle);
	const VAO dots;
	dots.Bind();
	const VBO dotVBO(particles,sizeof(particles));
	VAO::LinkAttrib(dotVBO,0,2,GL_FLOAT,sizeof(particle_t),nullptr);
	VAO::Unbind();
	smokeShader.setVec2("cent",glm::vec2((car->hitbox.x+0.037f+car->hitbox.x+0.037f+0.075f)/2,car->yOff));
	smokeShader.setVec3("col",0.1,0.1,0.1);

    const auto map_tileset = Texture("resources/test2.png",GL_TEXTURE_2D,GL_TEXTURE0,GL_UNSIGNED_BYTE,GL_REPEAT,0);
	map_tileset.Bind();
	map_tileset.texUnit(*myShader, "sheet");

	double spawnInterval = random(3,10);
	//double rocketInterval = random(3,10);

	double map1Loc=0,map2Loc=2;

	float speedMult = 1.0f;
	double blurTime=0;

	float w =0.0f,h=0.0f;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		const auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (pause == 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glClear(GL_COLOR_BUFFER_BIT);

			blurTime+=deltaTime;
			smokeShader.Activate();
			smokeShader.setVec2("cent",glm::vec2((car->hitbox.x+0.037f+car->hitbox.x+0.037f+0.075f)/2,car->yOff));
			myShader->Activate();
			if (currentFrame - prevEvent >= spawnInterval)
			{
				object_sizex = 0.27f;
				object_sizey = 0.27f;
				texpos=random(1,4);
				const char* ime = nullptr;
				if (texpos == 1) ime = "pecurka";
				else if (texpos == 2) ime = "potion";
				else if (texpos == 3) ime = "vinjak";
				else if (texpos == 4) {
					ime = "mud";
					texpos = 16;
				}
				verts = makeArray(object_sizex, object_sizey, texpos);
				auto* obj = new object(ime, -0.83f+random(0,148)/100,1.0f, verts.data(), carIndices, sizeof(float)*verts.size(),sizeof(carIndices),0.2f*deltaTime*speedMult);
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
				if (texpos == 16) {
					hb_x = obj->xOff;
					hb_y = obj->yOff+0.08f;
					hb_w = 0.27f*aspect;
					hb_h = 0.08f;
				}
				obj->setHitbox(hb_x,hb_y,hb_w,hb_h);
				if (texpos == 16) dangers.push_back(obj);
				else pickups.push_back(obj);
				spawnInterval = random(3,10);
				prevEvent = currentFrame;
			}
			/*if (crntTime - prevRaketa >= rocketInterval) {
				float cnt = random(3,7);
				for (int i = 0;i<cnt;i++) {
					object_sizex = 0.35f;
					object_sizey = 0.35f;
					//texpos=random(1,2);
					const char* ime = nullptr;
					verts = makeArray(object_sizex, object_sizey, random(17,17));
					auto* obj = new object("raketa", -0.9f+random(1,157)/100,1.0f, verts.data(), carIndices, sizeof(float)*verts.size(),sizeof(carIndices),0.5f*deltaTime*speedMult);
					obj->setHitbox(obj->xOff+0.095f,obj->yOff,0.07f*aspect,0.23f);
					dangers.push_back(obj);
				}
				rocketInterval = random(3,10);
				prevRaketa = crntTime;
			} */
			if (alfa < 0 && pecurkaActive==true) {
				pecurkaActive = false;
				myShader->setFloat("alfa",0.0f);
				smokeShader.Activate();
				smokeShader.setVec3("col",0.1,0.1,0.1);
				myShader->Activate();
			}
			else if (pecurkaActive==true) {
				alfa -= 0.08f*deltaTime;
				myShader->setFloat("alfa",alfa);
			}
			if (currentFrame - prevPotion >= 5.0f) potionFactor = 1.0f;
			if (currentFrame - prevSpeed >= 10.0f) {
				speedMult+=0.2f;
				for (object* obj : pickups) obj->fall = 0.2f*deltaTime*speedMult;
				for (object* obj : dangers) {
					if (obj->ime == "raketa") obj->fall = 0.5f*deltaTime*speedMult;
					else obj->fall = 0.2f*deltaTime*speedMult;
				}
				prevSpeed = currentFrame;
			}
			if (currentFrame - prevMud >= 5.0f) {
				mudActive = false;
				mudPos.clear();
			}
			if (mudActive) {
				glEnable(GL_STENCIL_TEST);
				glStencilMask(0xFF);
				glClear(GL_STENCIL_BUFFER_BIT);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				stencilShader.Activate();
				stencilVAO.Bind();
				for (auto mudP : mudPos) {
					auto model = glm::mat4(1.0f);
					model = glm::translate(model, mudP);
					stencilShader.setMat4("model", model);

					glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circleVerts)/sizeof(float));
				}
				glStencilMask(0x00);
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			}
			else glDisable(GL_STENCIL_TEST);

			myShader->Activate();
			map_tileset.Bind();

			map1Loc -= 0.2f*deltaTime*speedMult;
			map2Loc -= 0.2f*deltaTime*speedMult;

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
			for (object* obj : dangers) obj->draw();
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
					glm::sin(currentFrame*2.0f) * 0.5f + 0.5f,
					glm::sin(currentFrame*2.0f+ 2.094f) * 0.5f + 0.5f,
					glm::sin(currentFrame*2.0f+ 4.188f) * 0.5f + 0.5f);
			}
			else smokeShader.setVec3("col",0.1,0.1,0.1);
			dots.Bind();
			dotVBO.changeArray(particles,MAX_PARTICLES * sizeof(particle_t));
			glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
			myShader->Activate();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		if (blur && pause == 0) {
			const auto r = static_cast<float>(1.5f * blurTime);
			h = cos(r);
			w = sin(r);
			k-=10.0f*deltaTime;
			if (k<0) blur = false;
		}
		screenShader->Activate();
		screenShader->setVec2("vec",w*k,h*k);
		screennVAO.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete car;
	delete myShader;
	delete screenShader;

	VAO1.Delete();
    map1.Delete();
	VAO2.Delete();
	map2.Delete();
    myShader->Delete();
	screenShader->Delete();
	smokeShader.Delete();
	stencilShader.Delete();
    map_tileset.Delete();
	screennVAO.Delete();
	stencilVAO.Delete();
	stencilVBO.Delete();
	quadVBO.Delete();

    return 0;
}