#include "Includes.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Camera settings
//							  width, heigh, near plane, far plane
Camera_settings camera_settings{ 1500.0, 1200.0, 0.1, 500.0 };

//Timer
Timer timer;

// Instantiate the camera object with basic data
Camera camera(camera_settings, glm::vec3(0.0, 15.0, 52.0));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

bool pause = false;

//Global object position data makes it easier for updating object movement later
//Object position data for the white monsters and malboros
vector<float> objectXPositions =
{
	-14.0, //White monster1
	14.0, //White monster 2
	-14.0, //Malboro reds 1
	14.0 //Malboro reds 2
};

vector<float> objectYPositions =
{
	-10.0, //White monster1
	-10.0, //White monster 2
	10.0, //Malboro reds 1
	10.0 //Malboro reds 2
};

//Variables for navigating the object array
int objectPositionIndex = 0;
int maxObjectArrayIndex = 3;

vector<float> defaultObjectXPositions =
{
	-14.0, //White monster1
	14.0, //White monster 2
	-14.0, //Malboro reds 1
	14.0 //Malboro reds 2
};

vector<float> defaultObjectYPositions =
{
	-10.0, //White monster1
	-10.0, //White monster 2
	10.0, //Malboro reds 1
	10.0 //Malboro reds 2
};

//bool variable so it doesn't get wierd with selecting the different objects
bool keyPressed = false;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "30077548 Computer Graphics Assignment 2", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Rendering settings
	glfwSwapInterval(1);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);	//Enables depth testing
	glEnable(GL_CULL_FACE);		//Enables face culling
	glFrontFace(GL_CCW);		//Specifies which winding order if front facing

	////	Shaders - Textures - Models	////

	GLuint basicShader;
	GLuint modelShader;
	GLuint perPixelShader;
	GLuint perVertexShader;

	// Texture container
	GLuint metalTex = TextureLoader::loadTexture("Resources\\Models\\metal_texture.png");
	GLuint grassTexture = TextureLoader::loadTexture("Resources\\Models\\Grass\\grassTexture.jpg");

	// build and compile our shader program
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Basic_shader.vert"),
		string("Resources\\Shaders\\Basic_shader.frag"),
		&basicShader);

	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\ModelShader.vert"),
		string("Resources\\Shaders\\ModelShader.frag"),
		&modelShader);

	//perPixel shader
	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Per_Pixel.vert"),
		string("Resources\\Shaders\\Per_Pixel.frag"),
		&perPixelShader);

	//perVertex shader
	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Per_Vertex.vert"),
		string("Resources\\Shaders\\Per_Vertex.frag"),
		&perVertexShader);

	Model sphere("Resources\\Models\\Sphere.obj");
	Model rockySphere("Resources\\Models\\RockySphere\\RockySphere.obj");
	Model plane("Resources\\Models\\Plane.obj");
	Model japaneseTemple("Resources\\Models\\JapaneseTemple\\Japanese_Temple.obj");
	Model whiteMonster("Resources\\Models\\WhiteMonster\\whiteMonster.obj");
	Model malboroReds("Resources\\Models\\MalboroReds\\malboroReds.obj");

	SkinnedMesh eagle;

	eagle.LoadMesh("Resources\\Models\\Eagle\\eagle.fbx");

	const int numberOfBones = eagle.NumBones();
	vector<glm::mat4> Transforms;

	sphere.attachTexture(metalTex);
	plane.attachTexture(grassTexture);

	//Light Data///////////////////////////////////////////////
	// Lights
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };	// Dim light 
	GLfloat light_diffusers[] = {
		1.0, 0.0, 0.0, 1.0, // Red
		0.0, 1.0, 0.0, 1.0, // Green
		1.0, 1.0, 0.0, 1.0, //Yellow
		0.0, 0.0, 1.0, 1.0, //Blue
		1.0, 1.0, 1.0, 1.0 //White light above tower
	};	// White main light 
	GLfloat light_positions[] = {
		27.0, 5.0, 18.0, 1.0, //Red Light
		-27.0, 5.0, 18.0, 1.0, //Green
		-27.0, 5.0, -18.0, 1.0, //Yellow
		27.0, 5.0, -18.0, 1.0, // Blue
		0.0, 20.0, 0.0, 1.0 // White light above tower
	};	// Point light (w=1.0)
	GLfloat	attenuation[] = { 1.0, 0.10, 0.08 };
	
	// Materials
	GLfloat mat_amb_diff[] = { 1.0, 1.0, 1.0, 1.0 };	// Texture map will provide ambient and diffuse.
	GLfloat mat_specularCol[] = { 1.0, 1.0, 1.0, 1.0 }; // White highlight
	GLfloat mat_specularExp = 32.0;					// Shiny surface

	//Uniform Locations - Basic Shader////////////////////////////////////////////
	// Get unifom locations in shader
	GLuint uLightAmbient = glGetUniformLocation(basicShader, "lightAmbient");
	GLuint uLightDiffusers = glGetUniformLocation(basicShader, "lightDiffusers");
	GLuint uLightAttenuation = glGetUniformLocation(basicShader, "lightAttenuation");
	GLuint uLightPositions = glGetUniformLocation(basicShader, "lightPositions");
	GLuint uEyePos = glGetUniformLocation(basicShader, "eyePos");

	// Get material unifom locations in shader
	GLuint uMatAmbient = glGetUniformLocation(basicShader, "matAmbient");
	GLuint uMatDiffuse = glGetUniformLocation(basicShader, "matDiffuse");
	GLuint uMatSpecularCol = glGetUniformLocation(basicShader, "matSpecularColour");
	GLuint uMatSpecularExp = glGetUniformLocation(basicShader, "matSpecularExponent");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 sphereModel = glm::mat4(1.0);
		glm::mat4 rockySphereModel = glm::mat4(1.0);
		glm::mat4 whiteMonsterModel = glm::mat4(1.0);
		glm::mat4 malboroRedsModel = glm::mat4(1.0);
		glm::mat4 planeModel = glm::mat4(1.0);
		glm::mat4 japaneseTempleModel = glm::mat4(1.0);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();
		glm::vec3 eyePos = camera.getCameraPosition();

		//Taken from week 19 tutorial on spinning an object around
		static float phase = 0.0f; // variable to store the incrementing value
		if (!pause)
			phase += timer.getDeltaTimeSeconds() * glm::pi<float>() * 4.0;


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		//DRAW THE SPHERE OBJECT WITH ROCKY TEXTURE USING THE PER VERTEX SHADER
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		glUseProgram(perVertexShader);

		glUniformMatrix4fv(glGetUniformLocation(perVertexShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(perVertexShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		GLint perVertexLightDirectionLoc = glGetUniformLocation(perVertexShader, "lightDirection");
		GLint perVertexLightDiffuseLoc = glGetUniformLocation(perVertexShader, "lightDiffuseColour");
		glUniform4f(perVertexLightDirectionLoc, -27.0, 5.0, -18.0, 1.0);
		glUniform4f(perVertexLightDiffuseLoc, 0.0, 1.0, 0.0, 1.0);
		
		rockySphereModel = glm::translate(rockySphereModel, glm::vec3(-48.0, 5.0, -41.0));
		rockySphereModel = glm::rotate(rockySphereModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(perVertexShader, "model"), 1, GL_FALSE, glm::value_ptr(rockySphereModel));
		rockySphere.draw(perVertexShader);

		glUseProgram(0);
		///////////////////////////////////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// DRAW SPHERE OBJECT WITH METALLIC TEXTURE USING THE PER PIXEL SHADER
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		glUseProgram(perPixelShader);

		glUniformMatrix4fv(glGetUniformLocation(perPixelShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(perPixelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		GLint perPixelLightDirectionLoc = glGetUniformLocation(perPixelShader, "lightDirection");
		GLint perPixelLightDiffuseLoc = glGetUniformLocation(perPixelShader, "lightDiffuseColour");
		glUniform4f(perPixelLightDirectionLoc, 27.0, 5.0, 18.0, 1.0);
		glUniform4f(perPixelLightDiffuseLoc, 0.0f, 1.0f, 1.0f, 1.0f);

		sphereModel = glm::translate(sphereModel, glm::vec3(48.0, 5.0, 41.0));
		sphereModel = glm::rotate(sphereModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(perVertexShader, "model"), 1, GL_FALSE, glm::value_ptr(sphereModel));
		sphere.draw(perPixelShader); //Render the model

		glUseProgram(0);
		///////////////////////////////////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// DRAW THE DIFFERENT COLOUR LIGHTS, MONSTER'S AND MALBORO'S USING THE BASIC SHADER
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		glUseProgram(basicShader); //Use the Basic shader
		//Pass the uniform data to Basic shader///////////////////////////////////
		//Pass the light data
		glUniform4fv(uLightDiffusers, 5, (GLfloat*)&light_diffusers);
		glUniform4fv(uLightAmbient, 1, (GLfloat*)&light_ambient);
		glUniform4fv(uLightPositions, 5, (GLfloat*)&light_positions);
		glUniform3fv(uLightAttenuation, 1, (GLfloat*)&attenuation);
		glUniform3fv(uEyePos, 1, (GLfloat*)&eyePos);


		//Pass material data
		glUniform4fv(uMatAmbient, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(uMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(uMatSpecularCol, 1, (GLfloat*)&mat_specularCol);
		glUniform1f(uMatSpecularExp, mat_specularExp);

		glUniformMatrix4fv(glGetUniformLocation(basicShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		//Drawing the objects
		planeModel = glm::scale(glm::mat4(1.0), glm::vec3(5.0, 5.0, 5.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));
		plane.draw(basicShader); //Draw the plane

		japaneseTempleModel = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(japaneseTempleModel));
		japaneseTemple.draw(basicShader);

		//White monsters and malboro reds

		whiteMonsterModel = glm::scale(glm::mat4(1.0), glm::vec3(2.0, 2.0, 2.0));
		whiteMonsterModel = glm::translate(whiteMonsterModel, glm::vec3(objectXPositions[0], 1.0, objectYPositions[0]));
		whiteMonsterModel = glm::rotate(whiteMonsterModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(whiteMonsterModel));
		whiteMonster.draw(basicShader);

		whiteMonsterModel = glm::scale(glm::mat4(1.0), glm::vec3(2.0, 2.0, 2.0));
		whiteMonsterModel = glm::translate(whiteMonsterModel, glm::vec3(objectXPositions[1], 1.0, objectYPositions[1]));
		whiteMonsterModel = glm::rotate(whiteMonsterModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(whiteMonsterModel));
		whiteMonster.draw(basicShader);

		malboroRedsModel = glm::scale(glm::mat4(1.0), glm::vec3(2.0, 2.0, 2.0));
		malboroRedsModel = glm::translate(malboroRedsModel, glm::vec3(objectXPositions[2], 1.0, objectYPositions[2]));
		malboroRedsModel = glm::rotate(malboroRedsModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(malboroRedsModel));
		malboroReds.draw(basicShader);

		malboroRedsModel = glm::scale(glm::mat4(1.0), glm::vec3(2.0, 2.0, 2.0));
		malboroRedsModel = glm::translate(malboroRedsModel, glm::vec3(objectXPositions[3], 1.0, objectYPositions[3]));
		malboroRedsModel = glm::rotate(malboroRedsModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(malboroRedsModel));
		malboroReds.draw(basicShader);

		glUseProgram(0);
		///////////////////////////////////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// RENDER THE EAGLE USING THE MODEL SHADER
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		//Static time
		static float time = 0.0;
		time += timer.getDeltaTimeSeconds();
		float angle = glm::radians(30.0f) * time; // Adjust speed with 30.0f

		glm::mat4 eagleModel = glm::mat4(1.0);
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(rotation * glm::vec4(15.0, 12.0, 0.0, 1.0)));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.01, 0.01, 0.01));
		glm::mat4 facingRotation = glm::rotate(glm::mat4(1.0), angle + glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));

		// Combine transformations: translation first, then scale
		eagleModel = translation * facingRotation * scale;

		glUseProgram(modelShader); //Use the model shader

		glUniformMatrix4fv(glGetUniformLocation(modelShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(modelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(modelShader, "model"), 1, GL_FALSE, glm::value_ptr(eagleModel));

		//Animations are 0 - 2
		eagle.BoneTransform(time, Transforms, 1);

		for (int i = 0; i < numberOfBones; i++)
		{
			stringstream name;
			name << "gBones[" << i << "]";
			glUniformMatrix4fv(glGetUniformLocation(modelShader, name.str().c_str()), 1, GL_FALSE, glm::value_ptr(Transforms[i]));
		}

		eagle.Render(basicShader);

		glUseProgram(0);
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	double cameraMoveSpeed = timer.getDeltaTimeSeconds() * 8;
	double objectModeSpeed = 0.05;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, cameraMoveSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, cameraMoveSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, cameraMoveSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, cameraMoveSpeed);

	//process input for changing which object to control
	/*boolean added as without it it kept messing with which was the current object index as it was very sensitive on what
	the press needed to be from the keyboard*/
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (!keyPressed)
		{
			keyPressed = true;
			objectPositionIndex -= 1;

			if (objectPositionIndex < 0)
			{
				objectPositionIndex = maxObjectArrayIndex;
			}
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (!keyPressed)
		{
			keyPressed = true;
			objectPositionIndex += 1;

			if (objectPositionIndex > maxObjectArrayIndex)
			{
				objectPositionIndex = 0;
			}
		}
	}
	else
	{
		keyPressed = false;
	}

	//Code for changing x and y object positions
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		objectXPositions[objectPositionIndex] -= objectModeSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		objectXPositions[objectPositionIndex] += objectModeSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		objectYPositions[objectPositionIndex] -= objectModeSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		objectYPositions[objectPositionIndex] += objectModeSpeed;
	}

	//Code for resetting object positions as they were
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		objectXPositions = defaultObjectXPositions;
		objectYPositions = defaultObjectYPositions;
		objectPositionIndex = 0;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		camera.processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}