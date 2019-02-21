#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "shader_m.h"
#include "camera.h"
#include "shadow.h"

#include <iostream>

using vec3 = glm::vec3;
using quat = glm::quat;

#include "texture.h"
#include "transform.h"
#include "mesh_render.h"
#include "point_light.h"
#include "directional_light.h"


#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool showDebug = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_CULL_FACE);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("./shaders/color.vert", "./shaders/color.frag");
	Shader lampShader("./shaders/color.vert", "./shaders/lamp.frag");
	Shader shadowShader("./shaders/shadows.vert", "./shaders/shadows.frag");
	Shader gBufferShader("./shaders/gbuffer.vert", "./shaders/gbuffer.frag");
	Shader deferredLightingShader("./shaders/deferred_lighting.vert", "./shaders/deferred_lighting.frag");

	unsigned int testTexture = loadTexture("./textures/container2.png");
	unsigned int specularMap = loadTexture("./textures/container_specular.png");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions			// normals				// texture coords
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -	1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -	1.0f,  0.0f,  0.0f,		1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -	1.0f,  0.0f,  0.0f,		0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -	1.0f,  0.0f,  0.0f,		1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f,  1.0f
	};

	MeshRenderer boxRenderer, lampRenderer;
	MeshLibrary meshLibrary;
	TransformManager transforms;
	PointLightManager pointLights;
	DirectionalLightManager directionalLights;
	ShadowRenderManager shadowRenderer;

	boxRenderer.transforms = &transforms;
	pointLights.transforms = &transforms;
	directionalLights.transforms = &transforms;

	lampRenderer.transforms = &transforms;

	MeshData cubeMesh = meshLibrary.loadMesh("cube", vertices, sizeof(vertices));
	handle transformId = transforms.add(vec3(0, 0, 0), quat(), vec3(1, 1, 1));
	boxRenderer.add(transformId, cubeMesh);

	transformId = transforms.add(vec3(2, 0, 0), quat(), vec3(1, 1, 1));
	boxRenderer.add(transformId, cubeMesh);

	handle lampId = transforms.add(vec3(0, 2, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
	lampRenderer.add(lampId, cubeMesh);
	pointLights.add(lampId, 1, vec3(1), vec3(1));

	handle sunTransformId = transforms.add(vec3(0, 2, 0), quat(vec3(1, 1, 0)), vec3(0.1f, 0.1f, 0.1f));
	lampRenderer.add(sunTransformId, cubeMesh);
	directionalLights.add(sunTransformId, 1, vec3(1), vec3(1));

	shadowRenderer.shadowCasters = &boxRenderer;
	shadowRenderer.init();

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer generated" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool renderToBuffer = false;

	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gColorSpec;

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		transforms[lampId].position.x = cos(currentFrame / 10);
		transforms[lampId].position.y = 0;
		transforms[lampId].position.z = sin(currentFrame / 10);

		// input
		// -----
		processInput(window);
		if (!showDebug)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		}

		lightingShader.use();
		lightingShader.setInt("material.ambient", 0);
		lightingShader.setInt("material.specular", 1);
		lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingShader.setFloat("material.shininess", 30.0f);

		pointLights.assignShaderData(lightingShader.ID);
		directionalLights.assignShaderData(lightingShader.ID);

		// view/projection transformations fixed for all renderers
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);
		lightingShader.setVec3("viewPos", camera.Position);

		gBufferShader.use();
		gBufferShader.setMat4("projection", projection);
		gBufferShader.setMat4("view", view);
		gBufferShader.setMat4("model", model);
		gBufferShader.setVec3("viewPos", camera.Position);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindFramebuffer(GL_FRAMEBUFFER, renderToBuffer ? fbo : 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gBufferShader.use();
		boxRenderer.render(gBufferShader.ID);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Lighting pass

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);
		// also send light relevant uniforms
		/*shaderLightingPass.use();
		SendAllLightUniformsToShader(shaderLightingPass);
		shaderLightingPass.setVec3("viewPos", camera.Position);*/

		/*// 2. lighting pass: use g-buffer to calculate the scene's lighting
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		lightingPassShader.use();
		BindAllGBufferTextures();
		SetLightingUniforms();
		RenderQuad();*/


		// also draw the lamp object
		/*lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, transforms[lampId].position);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		lampRenderer.render(lampShader.ID);*/
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//shadowRenderer.render(shadowShader.ID);
		/*glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		if (showDebug)
		{
			if(ImGui::Begin("Debug Window", &showDebug));
			{
				static vec3 euler(0, 0, 0);
				static unsigned int displayTexture = 0;
				if (ImGui::DragFloat3("Sun direction", (float*)&euler, 0.01f))
				{
					transforms[sunTransformId].rotation = quat(euler);
				}
				ImGui::Image((ImTextureID)displayTexture, ImVec2(400, 300));
				
				char buffer[500];
				sprintf_s(buffer, "%d", displayTexture);
				if (ImGui::BeginCombo("Texture", buffer))
				{
					if (ImGui::Selectable("Position"))
					{
						displayTexture = gPosition;
					}
					if (ImGui::Selectable("Colors"))
					{
						displayTexture = gColorSpec;
					}
					if (ImGui::Selectable("Normal"))
					{
						displayTexture = gNormal;
					}
					ImGui::EndCombo();
				}
				
				if (ImGui::Button("Render to Screen"))
				{
					renderToBuffer = false;
				}
				if (ImGui::Button("Render to FrameBuffer"))
				{
					renderToBuffer = true;
				}
				if (ImGui::Checkbox("Show Debug", &showDebug))
				{
					ImGui::ShowDemoWindow(&showDebug);
				}
				ImGui::End();
			}
		}
		ImGui::Render();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (!showDebug)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	static bool spacePressed = false;
	if (spacePressed)
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
			spacePressed = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		showDebug = !showDebug;
		spacePressed = true;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (!showDebug)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}