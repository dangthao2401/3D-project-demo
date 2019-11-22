// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

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
	GLuint modelID;
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
	glm::vec3 rotate_Asix;//an axis of 3 scalars X,Y,Z
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

void update_object(Object object, float deltaTime)
{
	
}
void update_camera(Camera camera, float deltaTime)
{
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera.position += deltaTime * camera.speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera.position -= deltaTime * camera.speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera.position += deltaTime * camera.speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera.position -= deltaTime * camera.speed;
	}
}


glm::mat4 calCulateModelMatrix(Object object)
{
	// Transform object 1 to World Space
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), object.position);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), object.scale);
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
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

	GLuint TextureLoc[5];
	for (GLuint i = 0; i < object.number_Textures; i++)
	{
		TextureLoc[i] = glGetUniformLocation(programID, "texture" + i);
	}
	GLuint CubeTextureLoc = glGetUniformLocation(programID, "cubemap");
	GLuint material_ambient_Loc = glGetUniformLocation(programID, "material.ambientColor");
	GLuint material_diffuse_Loc = glGetUniformLocation(programID, "material.diffuseColor");
	GLuint material_specular_Loc = glGetUniformLocation(programID, "material.specularColor");
	GLuint material_shininess_Loc = glGetUniformLocation(programID, "material.shininess");

	// Compute the MVP matrix from keyboard and mouse input;
	glm::vec3 ViewPos = camera.position;
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	ModelMatrix = calCulateModelMatrix(object);
	glm::mat4 MVP = camera.projectionMatrix * camera.viewMatrix * ModelMatrix;


	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(mpvLoc, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixLoc, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixLoc, 1, GL_FALSE, &camera.viewMatrix[0][0]);
	glUniformMatrix4fv(ProjectionMatrixLoc, 1, GL_FALSE, &camera.projectionMatrix[0][0]);

	// Bind our texture in Texture Unit 
	if (object.number_Textures > 0)
	{
		for (GLuint i = 0; i < object.number_Textures; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, object.TextureIDs[i]);
			glUniform1i(TextureLoc[i], GL_TEXTURE0 + i);
		}
	}
	if (object.number_CubeTextures > 0)
	{
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
	glDrawArrays(GL_TRIANGLES, 0, model_object.vertices.size());

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
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Load model
	GLuint number_models = 2;
	Model models[2];
	bool isloadingModel1 = loadOBJf34("data/model/Moon 2K.obj", models[0].vertices, models[0].uvs, models[0].normals);
	bool isloadingModel2 = loadOBJf33("data/model/cube.obj", models[1].vertices, models[1].uvs, models[1].normals);

	// Create and compile our GLSL program from the shaders
	GLuint number_programIDs = 1;
	GLuint programIDs[2];
	programIDs[0] = LoadShaders("shaders/Solar.vertexshader", "shaders/Solar.fragmentshader");
	programIDs[1] = LoadShaders("shaders/CubeMap.vertexshader", "shaders/CubeMap.fragmentshader");

	// Load Textures
	int number_Textures = 2;
	GLuint TextureIDs[5];
	//Earth texture
	TextureIDs[0] = loadPNG("data/texture/Diffuse_Earth_2K.png");
	//Moon texture
	TextureIDs[1] = loadPNG("data/texture/Diffuse_Moon_2K.png");

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
	GLuint number_Object = 1;
	Object objects[4];
	
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
	
	//---------------------Object 1------	-------------------------------
	objects[0].position = glm::vec3(-1.5f, 0.0f, 0.0f);
	objects[0].scale = glm::vec3(0.3f, 0.3f, 0.3f);
	objects[0].rotation = glm::vec3(1.0);
	objects[0].programID = 0;
	objects[0].modelID = 0;
	objects[0].number_Textures = 1;
	objects[0].TextureIDs[0] = TextureIDs[0];// moon
	objects[0].number_CubeTextures = 0;
	objects[0].lightID = 0;
	objects[0].materialID = 0;
	objects[0].speed = 3.0f;
	//objects[0].roa = glm::vec3(0.0f, 1.0f, 0.0f); // Y
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

	//45° Field of View, 4:3 ratio, display range : 0.1 unit < -> 100 units
	camera.fovy = 45.0f;
	camera.aspect = 4.0f / 3.0f;
	camera.nearPlan = 1.0f;
	camera.farPlan = 1000.0f;
	camera.viewMatrix = glm::lookAt(camera.position, camera.target, camera.up);
	camera.projectionMatrix = glm::perspective(glm::radians(camera.fovy), camera.aspect, camera.nearPlan, camera.farPlan);
	static double lastTime = glfwGetTime();
	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glfwGetTime is called only once, the first time this function is called
		// Compute time difference between current and last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		for (GLuint i = 0; i < number_Object; i++)
		{

			Model model_object = models[objects[i].modelID];
			Light light_object = lights[objects[i].lightID];
			Material material_object = materials[objects[i].materialID];
			GLuint programID = programIDs[objects[i].programID];
			update_object(objects[i], deltaTime);
			draw_object(objects[i], model_object, light_object, programID, material_object, camera);
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

	for (int i = 0; i < number_Textures; i++)
	{
		glDeleteTextures(1, &TextureIDs[i]);
	}
	for (int i = 0; i < number_CubeTextures; i++)
	{
		glDeleteTextures(1, &CubeTextureIDs[i]);
	}
	for (int i = 0; i < number_programIDs; i++)
	{
		glDeleteProgram(programIDs[i]);
	}
	for (int i = 0; i < number_programIDs; i++)
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

