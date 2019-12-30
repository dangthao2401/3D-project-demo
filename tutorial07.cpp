// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include  <map>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include "common/nfgloader.hpp"

struct Light {
	glm::vec3 position;
	glm::vec3 color;
};

struct Material {
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	GLfloat shininess;
};

struct Model {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};
struct Object {
	GLuint modelID;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	GLuint programID;
	GLuint number_Textures;
	GLuint TextureIDs[4];
	GLuint number_CubeTextures;
	GLuint CubeTextureIDs[4];
	GLuint lightID;
	GLuint materialID;
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLfloat speed;
	GLfloat rotate_angle_in_degrees;
	bool selected;
	bool isTransparent;
};
struct Camera {
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;
	GLfloat fovy;
	GLfloat aspect;
	GLfloat nearPlan;
	GLfloat farPlan;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	GLfloat speed;
	
};

// Create programID for selected shader
GLint selected_programID;
float time;
void update_object(Object object, float deltaTime)
{
	
}
glm::vec3 update_camera(Camera camera, float deltaTime)
{
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera.position.b += deltaTime * camera.speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera.position.b -= deltaTime * camera.speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera.position.r += deltaTime * camera.speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera.position.r -= deltaTime * camera.speed;
	}
	return camera.position;
}


glm::mat4 calCulateModelMatrix(Object object)
{
	// Transform object 1 to World Space
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), object.position);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), object.scale);
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), object.rotate_angle_in_degrees, object.rotation);
	glm::mat4  ModelMatrix = translateMatrix * rotationMatrix * scaleMatrix;
	return ModelMatrix;
}
void draw_object(Object object, Model model_object, Light light_object, GLuint programID, Material material_object, Camera camera)
{
	// Use our shader
	glUseProgram(programID);
	// create VAO
	glGenVertexArrays(1, &object.VertexArrayID);
	glBindVertexArray(object.VertexArrayID);

	glGenBuffers(1, &object.vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, model_object.vertices.size() * sizeof(glm::vec3), &model_object.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &object.uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, model_object.uvs.size() * sizeof(glm::vec2), &model_object.uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &object.normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, model_object.normals.size() * sizeof(glm::vec3), &model_object.normals[0], GL_STATIC_DRAW);

	// Get a handle for our uniform
	GLuint mpvLoc = glGetUniformLocation(programID, "MVP");
	GLuint ModelMatrixLoc = glGetUniformLocation(programID, "ModelMatrix");
	GLuint ViewMatrixLoc = glGetUniformLocation(programID, "ViewMatrix");
	GLuint ProjectionMatrixLoc = glGetUniformLocation(programID, "ProjectionMatrix");
	GLuint ViewPosLoc = glGetUniformLocation(programID, "viewPos_worldspace");
	GLuint lightPosLoc = glGetUniformLocation(programID, "light.position");
	GLuint lightColorLoc = glGetUniformLocation(programID, "light.color");

	GLuint timeLoc = glGetUniformLocation(programID, "u_time");

	GLuint TextureLoc[5];
	//TextureLoc[0] = glGetUniformLocation(programID, "fire_texture");
	//TextureLoc[1] = glGetUniformLocation(programID, "disp_texture");
	//TextureLoc[2] = glGetUniformLocation(programID, "alpha_texture");
	/*TextureLoc[0] = glGetUniformLocation(programID, "grass_texture");
	TextureLoc[1] = glGetUniformLocation(programID, "dirt_texture");
	TextureLoc[2] = glGetUniformLocation(programID, "rock_texture");
	TextureLoc[3] = glGetUniformLocation(programID, "blending_texture");*/

	TextureLoc[0] = glGetUniformLocation(programID, "texture0");

	GLuint CubeTextureLoc = glGetUniformLocation(programID, "cubemap");
	GLuint material_ambient_Loc = glGetUniformLocation(programID, "material.ambientColor");
	GLuint material_diffuse_Loc = glGetUniformLocation(programID, "material.diffuseColor");
	GLuint material_specular_Loc = glGetUniformLocation(programID, "material.specularColor");
	GLuint material_shininess_Loc = glGetUniformLocation(programID, "material.shininess");
/*
	// Compute the MVP matrix from keyboard and mouse input;
	glm::vec3 ViewPos = camera.position;
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	ModelMatrix = calCulateModelMatrix(object);
	glm::mat4 MVP = camera.projectionMatrix * camera.viewMatrix * ModelMatrix;
	*/
	// Compute the MVP matrix from keyboard and mouse input
	glm::vec3 ViewPos =  computeMatricesFromInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = calCulateModelMatrix(object);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(mpvLoc, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixLoc, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixLoc, 1, GL_FALSE, &camera.viewMatrix[0][0]);
	glUniformMatrix4fv(ProjectionMatrixLoc, 1, GL_FALSE, &camera.projectionMatrix[0][0]);

	// Bind our texture in Texture Unit 
	/*
	if (object.number_Textures > 0)
	{
		for (GLuint i = 0; i < object.number_Textures; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, object.TextureIDs[i]);
			glUniform1i(TextureLoc[i], GL_TEXTURE0 + i);
		}
	}
	*/
	glActiveTexture(GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, object.TextureIDs[0]);
	glUniform1i(TextureLoc[0], 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, object.TextureIDs[1]);
	glUniform1i(TextureLoc[1], 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, object.TextureIDs[2]);
	glUniform1i(TextureLoc[2], 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, object.TextureIDs[3]);
	glUniform1i(TextureLoc[3], 3);

	if (object.number_CubeTextures > 0)
	{
		//glDisable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, object.CubeTextureIDs[0]);
		glUniform1i(CubeTextureLoc, 0);
	}
	/*Material pos*/
	glUniform3f(material_ambient_Loc, material_object.ambientColor.x, material_object.ambientColor.y, material_object.ambientColor.z);
	glUniform3f(material_diffuse_Loc, material_object.diffuseColor.x, material_object.diffuseColor.y, material_object.diffuseColor.z);
	glUniform3f(material_specular_Loc, material_object.specularColor.x, material_object.specularColor.y, material_object.specularColor.z);
	glUniform1f(material_shininess_Loc, material_object.shininess);


	// lightPos
	glUniform3f(lightPosLoc, light_object.position.x, light_object.position.y, light_object.position.z);
	glUniform3f(lightColorLoc, light_object.color.x, light_object.color.y, light_object.color.z);
	// viewPos
	glUniform3f(ViewPosLoc, ViewPos.x, ViewPos.y, ViewPos.z);

	// time 
	glUniform1f(timeLoc, time);

	// 1st. render pass, draw objects as normal, writing to the stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, object.vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, object.uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	// 3nd 
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, object.normalbuffer);
	glVertexAttribPointer(
		2,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_TRUE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);


	// Draw the triangle !
	int numbervertices = model_object.vertices.size();
	glDrawArrays(GL_TRIANGLES, 0, numbervertices);
	// draw border
	if (object.selected == true)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(selected_programID);
		GLuint mpvLoc = glGetUniformLocation(selected_programID, "MVP");
		float scale = 1.1;
		ModelMatrix= glm::scale(ModelMatrix, glm::vec3(scale, scale, scale));
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(mpvLoc, 1, GL_FALSE, &MVP[0][0]);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, object.vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, numbervertices);
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
		//glDisableVertexAttribArray(0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
}
void delete_object(Object object)
{

	glDeleteBuffers(1, &object.vertexbuffer);
	glDeleteBuffers(1, &object.uvbuffer);
	glDeleteVertexArrays(1, &object.VertexArrayID);

}
void createquad(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals)
{
	glm::vec3 vertex1 = glm::vec3(-1.0f, -1.0f, 0.0f);
	vertices.push_back(vertex1);
	glm::vec3 vertex2 = glm::vec3(1.0f, -1.0f, 0.0f);
	vertices.push_back(vertex2);
	glm::vec3 vertex3 = glm::vec3(1.0f, 1.0f, 0.0f);
	vertices.push_back(vertex3);
	glm::vec3 vertex4 = glm::vec3(-1.0f, -1.0f, 0.0f);
	vertices.push_back(vertex4);
	glm::vec3 vertex5 = glm::vec3(1.0f, 1.0f, 0.0f);
	vertices.push_back(vertex5);
	glm::vec3 vertex6 = glm::vec3(-1.0f, 1.0f, 0.0f);
	vertices.push_back(vertex6);

	glm::vec2 uv1 = glm::vec2(0.0f, 0.0f);
	uvs.push_back(uv1);
	glm::vec2 uv2 = glm::vec2(1.0f, 0.0f);
	uvs.push_back(uv2);
	glm::vec2 uv3 = glm::vec2(1.0f, 1.0f);
	uvs.push_back(uv3);
	glm::vec2 uv4 = glm::vec2(0.0f, 0.0f);
	uvs.push_back(uv4);
	glm::vec2 uv5 = glm::vec2(1.0f, 1.0f);
	uvs.push_back(uv5);
	glm::vec2 uv6 = glm::vec2(0.0f, 1.0f);
	uvs.push_back(uv6);

	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);
	normals.push_back(normal);

}
int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Enable stencil test
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	// Load model
	GLuint number_models = 3;
	Model models[3];
//	bool isloadingModel1 = loadOBJf34("data/model/ground.obj", models[0].vertices, models[0].uvs, models[0].normals);
	//bool isloadingModel2 = loadOBJf33("data/model/cube.obj", models[1].vertices, models[1].uvs, models[1].normals);
	createquad(models[0].vertices, models[0].uvs, models[0].normals);
	//bool res = loadnfg("data/model/Terrain.nfg", models[0].vertices, models[0].uvs, models[0].normals);
	//bool isloadingModel2 = loadOBJf33("data/model/cube.obj", models[1].vertices, models[1].uvs, models[1].normals);

	// Create and compile our GLSL program from the shaders
	GLuint number_programIDs = 3;
	GLuint programIDs[3];
	//programIDs[0] = LoadShaders("shaders/Solar.vertexshader", "shaders/Solar.fragmentshader");
	//programIDs[1] = LoadShaders("shaders/CubeMap.vertexshader", "shaders/CubeMap.fragmentshader");
	//programIDs[0] = LoadShaders("shaders/TextureFragmentShader.vertexshader", "shaders/TextureFragmentShader.fragmentshader");
	programIDs[0] = LoadShaders("shaders/Fog.vertexshader", "shaders/Fog.fragmentshader");
	//programIDs[1] = LoadShaders("shaders/CubeMap.vertexshader", "shaders/CubeMap.fragmentshader");
	programIDs[1] = LoadShaders("shaders/Blending.vertexshader", "shaders/Blending.fragmentshader");
	//programIDs[2] = LoadShaders("shaders/ TransparentWindow.vertexshader", "shaders/ TransparentWindow.fragmentshader");
	//selected_programID = LoadShaders("shaders/Border.vertexshader", "shaders/Border.fragmentshader");
	// Load Textures
	int number_Textures = 5;
	GLuint TextureIDs[5];
	//TextureIDs[0] = loadPNG("data/texture/fire/fire3.png",1); //1 = CLAMP_TO_EDGE
	//Moon texture
	//TextureIDs[1] = loadPNG("data/texture/fire/DisplacementMap.png",2);// 2 = REPEAT
	//Grass
	//TextureIDs[2] = loadPNG("data/texture/fire/fire_mask_1.png",1);//1 = CLAMP_TO_EDGE
	/*
	TextureIDs[0] = loadPNG("data/texture/terrain/Grass.png", 2);
	TextureIDs[1] = loadPNG("data/texture/terrain/Dirt.png", 2);
	TextureIDs[2] = loadPNG("data/texture/terrain/Rock.png", 2);
	TextureIDs[3] = loadPNG("data/texture/terrain/Blending.png", 2);
	*/
	/*
	TextureIDs[0] = createTeture2D("data/texture/terrain/Grass.tga");
	TextureIDs[1] = createTeture2D("data/texture/terrain/Dirt.tga");
	TextureIDs[2] = createTeture2D("data/texture/terrain/Rock.tga");
	TextureIDs[3] = createTeture2D("data/texture/Terrain_blendmap_1.tga");
	*/
	TextureIDs[0] = loadPNG("data/texture/ground.png",2);
	TextureIDs[1] = loadPNG("data/texture/Grass.png",2);
	TextureIDs[2] = loadPNG("data/texture/flowers.png", 2);
	// Load Cube
	int number_CubeTextures = 1;
	//6 faces of skybox
	char * faces[6];
	faces[0] = "data/texture/skybox/right.png";
	faces[1] = "data/texture/skybox/left.png";
	faces[2] = "data/texture/skybox/top.png";
	faces[3] = "data/texture/skybox/bottom.png";
	faces[4] = "data/texture/skybox/back.png";
	faces[5] = "data/texture/skybox/front.png";
	
	GLuint CubeTextureIDs[2];
	CubeTextureIDs[0] = loadPNG_CubeMap(faces);

	//Light position
	GLuint number_Light = 1;
	Light lights[2];
	lights[0].color = glm::vec3(1.0f, 1.0f, 1.0f);
	lights[0].position = glm::vec3(3.0f, 3.0f, 3.0f);

	/*Material creation*/
	Material materials[3];
	//emerald
	materials[0].ambientColor = glm::vec3(0.0215f, 0.1745f, 0.0215f);
	materials[0].diffuseColor = glm::vec3(0.07568f, 0.61424f, 0.07568f);
	materials[0].specularColor = glm::vec3(0.633f, 0.727811f, 0.633f);
	materials[0].shininess = 0.6f*128.0;

	materials[1].ambientColor = glm::vec3(0.24725f, 0.1995f, 0.0745f);
	materials[1].diffuseColor = glm::vec3(0.75164f, 0.60648f, 0.22648f);
	materials[1].specularColor = glm::vec3(0.628281f, 0.555802f, 0.366065f);
	materials[1].shininess = 0.4f*128.0;

	/*Create object*/
	GLuint number_Object = 6;
	Object objects[6];
	
	
	/*
	objects[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[0].scale = glm::vec3(3.0f, 3.0f, 3.0f);
	objects[0].rotation = glm::vec3(1.0);
	objects[0].programID = 1;
	objects[0].modelID = 1;
	objects[0].number_Textures = 0;
	objects[0].number_CubeTextures = 1;
	objects[0].CubeTextureIDs[0] = CubeTextureIDs[0];
	objects[0].lightID = 0;
	objects[0].materialID = 0;
	*/
	objects[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[0].scale = glm::vec3(5.0f);
	objects[0].rotation = glm::vec3(1.0f, 0.0f, 0.0f);// rotate round X Axis
	objects[0].rotate_angle_in_degrees = 90.0f;
	objects[0].programID = 0;
	objects[0].modelID = 0;
	objects[0].number_Textures = 1;
	objects[0].TextureIDs[0] = TextureIDs[0];
	objects[0].number_CubeTextures = 0;
	objects[0].lightID = 0;
	objects[0].materialID = 0;
	objects[0].selected = false;
	objects[0].isTransparent = false;
	// GROUND
	/*
	objects[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[0].scale = glm::vec3(5.0f);
	objects[0].rotation = glm::vec3(1.0f,0.0f,0.0f);// rotate round X Axis
	objects[0].rotate_angle_in_degrees =90.0f;
	objects[0].programID = 0;
	objects[0].modelID = 0;
	objects[0].number_Textures = 4;
	objects[0].TextureIDs[0] = TextureIDs[0];
	objects[0].TextureIDs[1] = TextureIDs[1];
	objects[0].TextureIDs[2] = TextureIDs[2];
	objects[0].TextureIDs[3] = TextureIDs[3];
	objects[0].number_CubeTextures = 0;
	objects[0].lightID = 0;
	objects[0].materialID = 0;
	objects[0].selected = false;
	objects[0].isTransparent = false;
	*/
	
	//GRASS 1
	objects[1].position = glm::vec3(0.0f, 0.0f, 2.0f);
	objects[1].scale = glm::vec3(1.0f);
	objects[1].rotation = glm::vec3(1.0);
	objects[1].programID = 1;
	objects[1].modelID = 0;
	objects[1].number_Textures = 1;
	objects[1].TextureIDs[0] = TextureIDs[1];
	objects[1].number_CubeTextures = 0;
	objects[1].lightID = 0;
	objects[1].materialID = 0;
	objects[1].selected = false;
	objects[1].isTransparent = false;

	//GRASS 2
	objects[2].position = glm::vec3(-2.0f, 0.0f, 2.0f);
	objects[2].scale = glm::vec3(1.0f);
	objects[2].rotation = glm::vec3(1.0);
	objects[2].programID = 1;
	objects[2].modelID = 0;
	objects[2].number_Textures = 1;
	objects[2].TextureIDs[0] = TextureIDs[1];
	objects[2].number_CubeTextures = 0;
	objects[2].lightID = 0;
	objects[2].materialID = 0;
	objects[2].selected = false;
	objects[2].isTransparent = false;

	//GRASS 4
	objects[3].position = glm::vec3(-4.0f, 2.0f,- 2.0f);
	objects[3].scale = glm::vec3(1.0f);
	objects[3].rotation = glm::vec3(1.0);
	objects[3].programID = 1;
	objects[3].modelID = 0;
	objects[3].number_Textures = 1;
	objects[3].TextureIDs[0] = TextureIDs[1];
	objects[3].number_CubeTextures = 0;
	objects[3].lightID = 0;
	objects[3].materialID = 0;
	objects[3].selected = false;
	objects[3].isTransparent = false;

	//GRASS 5
	objects[4].position = glm::vec3(0.0f, 2.0f, -2.0f);
	objects[4].scale = glm::vec3(1.0f);
	objects[4].rotation = glm::vec3(1.0);
	objects[4].programID = 1;
	objects[4].modelID = 0;
	objects[4].number_Textures = 1;
	objects[4].TextureIDs[0] = TextureIDs[1];
	objects[4].number_CubeTextures = 0;
	objects[4].lightID = 0;
	objects[4].materialID = 0;
	objects[4].selected = false;
	objects[4].isTransparent = false;

	//GRASS 6
	objects[5].position = glm::vec3(2.0f, 0.0f, 2.0f);
	objects[5].scale = glm::vec3(1.0f);
	objects[5].rotation = glm::vec3(1.0);
	objects[5].programID = 1;
	objects[5].modelID = 0;
	objects[5].number_Textures = 1;
	objects[5].TextureIDs[0] = TextureIDs[1];
	objects[5].number_CubeTextures = 0;
	objects[5].lightID = 0;
	objects[5].materialID = 0;
	objects[2].selected = false;
	objects[2].isTransparent = false;
	
	/*
	
	//---------------------Object 2------	-------------------------------
	objects[1].position = glm::vec3(-1.5f, 0.0f, 0.0f);
	objects[1].scale = glm::vec3(10.0f);
	objects[1].rotation = glm::vec3(1.0f);
	objects[1].programID = 1;
	objects[1].modelID = 1;
	objects[1].number_Textures = 0;
	objects[1].number_CubeTextures = 1;
	objects[1].CubeTextureIDs[0] = 0;
	objects[1].lightID = 0;
	objects[1].materialID = 0;
	objects[1].border = false;
	/*
	//---------------------Object 2-------------------------------------
	objects[1].position = glm::vec3(1.0f, 0.0f, 0.0f);
	objects[1].scale = glm::vec3(0.3f, 0.3f, 0.3f);
	objects[1].rotation = glm::vec3(1.0);
	objects[1].programID = 0;
	objects[1].modelID = 0;
	objects[1].number_Textures = 1;
	objects[1].TextureIDs[0] = 1;// earth
	objects[1].number_CubeTextures = 0;
	objects[1].lightID = 0;
	objects[1].materialID = 0;
	//---------------------Object 3-------------------------------------
	objects[2].position = glm::vec3(1.0f, -1.5f, -1.0f);
	objects[2].scale = glm::vec3(0.3f, 0.3f, 0.3f);
	objects[2].rotation = glm::vec3(1.0);
	objects[2].programID = 1;
	objects[2].modelID = 1;
	objects[2].number_Textures = 0;
	objects[2].number_CubeTextures = 1;
	objects[2].CubeTextureIDs[0] = 0 ;
	objects[2].lightID = 0;
	objects[2].materialID = 0;
	*/
	/*Create Camera*/
	Camera camera;
	camera.position = glm::vec3(0, 0, 5);
	camera.target = glm::vec3(0, 0, 0);
	camera.up = glm::vec3(0,1,0);
	camera.speed = 3.0f;

	//45° Field of View, 4:3 ratio, display range : 0.1 unit < -> 100 units
	camera.fovy = 45.0f;
	camera.aspect = 4.0f / 3.0f;
	camera.nearPlan = 1.0f;
	camera.farPlan = 1000.0f;
	camera.projectionMatrix = glm::perspective(glm::radians(camera.fovy), camera.aspect, camera.nearPlan, camera.farPlan);
	static double lastTime = glfwGetTime();
	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// glfwGetTime is called only once, the first time this function is called
		// Compute time difference between current and last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		time += deltaTime;
		std::map<float, Object> sorted;
		for (GLuint i = 0; i < number_Object; i++)
		{
			//Draw all opaque object
			if (objects[i].isTransparent == false)
			{
				Model model_object = models[objects[i].modelID];
				Light light_object = lights[objects[i].lightID];
				Material material_object = materials[objects[i].materialID];
				GLuint programID = programIDs[objects[i].programID];
				draw_object(objects[i], model_object, light_object, programID, material_object, camera);
			}
			else
			{
				// get transparent object and sort it by the distance from camera to object
				glm::vec3 CameraPos = computeMatricesFromInputs();
				float distance = glm::length(CameraPos - objects[i].position);
				sorted[distance] = objects[i];
			}
		}
		// Draw transparent object
		for (std::map<float, Object>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			Object transparentObject = it->second;
			Model model_object = models[transparentObject.modelID];
			Light light_object = lights[transparentObject.lightID];
			Material material_object = materials[transparentObject.materialID];
			GLuint programID = programIDs[transparentObject.programID];
			draw_object(transparentObject, model_object, light_object, programID, material_object, camera);
		}
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		// For the next frame, the "last time" will be "now"
		lastTime = currentTime;

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup 

	for (GLuint i = 0; i < number_Textures; i++)
	{
		glDeleteTextures(1, &TextureIDs[i]);
	}
	for (GLuint i = 0; i < number_CubeTextures; i++)
	{
		glDeleteTextures(1, &CubeTextureIDs[i]);
	}
	for (GLuint i = 0; i < number_programIDs; i++)
	{
		glDeleteProgram(programIDs[i]);
	}
	for (int i = 0; i < number_Object; i++)
	{
		delete_object(objects[i]);
	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

