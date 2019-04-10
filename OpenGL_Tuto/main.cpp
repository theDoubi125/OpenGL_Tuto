#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "shader_m.h"
#include "camera.h"

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

#include "render/render_pipeline.h"
#include "shadow.h"
#include "gameplay/rotation.h"
#include "util/bit_array.h"

#include "util/debug/table_display.h"
#include "gameplay/movement/cube_movement.h"

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
bool pauseGame = false;

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
	//glEnable(GL_CULL_FACE);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("./shaders/color.vert", "./shaders/color.frag");
	Shader lampShader("./shaders/color.vert", "./shaders/lamp.frag");
	Shader shadowShader("./shaders/shadows.vert", "./shaders/shadows.frag");
	Shader pointLightShader("./shaders/point_light.vert", "./shaders/point_light.frag");
	Shader directionalLightShader("./shaders/directional_light.vert", "./shaders/directional_light.frag");

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

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,		1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,		0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,		1.0f,  0.0f,

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
	PointLightManager pointLights;
	DirectionalLightManager directionalLights;
	ShadowRenderManager shadowRenderer;

	transform::init();
	rotation::anchor::init();
	rotation::animation::init();
	movement::cube::init();

	MeshData cubeMesh = meshLibrary.loadMesh("cube", vertices, sizeof(vertices));
	handle transformId = transform::add(vec3(0, 0, 0), quat(vec3(0, 0, -glm::pi<float>() / 3)), vec3(1, 1, 1));
	boxRenderer.add(transformId, cubeMesh);

	transformId = transform::add(vec3(2, 0, 0), quat(), vec3(1, 1, 1));
	boxRenderer.add(transformId, cubeMesh);

	transformId = transform::add(vec3(-2, 0, 0), quat(), vec3(1, 1, 1));
	boxRenderer.add(transformId, cubeMesh);

	transformId = transform::add(vec3(0, -1, 0), quat(), vec3(10, 1, 10));
	boxRenderer.add(transformId, cubeMesh);

	handle characterTransformId = transform::add(vec3(0, 0, 2), quat(), vec3(1, 1, 1));
	boxRenderer.add(characterTransformId, cubeMesh);
	handle cubeMovementId = movement::cube::add(characterTransformId, 0.5f);
	//rotation::animation::add(characterTransformId, vec3(0.5, -0.5, 0), vec3(0.5, -0.5, 0), 3, quat(), quat(vec3(0, 0, -glm::pi<float>() / 2)));

	handle lampId = transform::add(vec3(0, 2, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
	lampRenderer.add(lampId, cubeMesh);
	//pointLights.add(lampId, 1, vec3(1), vec3(1));

	handle sunTransformId = transform::add(vec3(0, 1, 0), quat(vec3(1, 1, 0)), vec3(0.1f, 0.1f, 0.1f));
	lampRenderer.add(sunTransformId, cubeMesh);

	handle testTransformId = transform::add(vec3(0, 0, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
	lampRenderer.add(testTransformId, cubeMesh);
	//pointLights.add(sunTransformId, 1, vec3(1), vec3(1));

	vec3 sunDirection(1, 0.7f, 0.45f);

	directionalLights.add(sunTransformId, 1, vec3(1), vec3(1));
	shadowRenderer.shadowCasters = &boxRenderer;
	shadowRenderer.init();

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
	

	render::init(SCR_WIDTH, SCR_HEIGHT);

	lastFrame = glfwGetTime();

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

		// input
		// -----
		processInput(window);

		int transformIndex = transform::indexOf(lampId);
		vec3& position = transform::positions[transformIndex];
		position.x = cos(currentFrame / 10);
		position.y = 0;
		position.z = sin(currentFrame / 10);
		if (!showDebug)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		}

		// view/projection transformations fixed for all renderers
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		static glm::vec4 testVec(0, 0, 0, 1);
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);

		if (pauseGame)
		{
			deltaTime = 0;
		}

		vec3 input(0, 0, 0);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			input.z = -1;
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			input.z = 1;
		else
			input.z = 0;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			input.x = -1;
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			input.x = 1;
		else
			input.x = 0;
		movement::cube::cubeInput[cubeMovementId.id] = -camera.Front * input.z + camera.Right * input.x;

		rotation::animation::update(deltaTime);
		rotation::anchor::update();
		movement::cube::update();

		render::clear_frame();
		render::start_render(camera);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		int diffuseAttr = glGetUniformLocation(render::currentShader, "diffuse");
		int specularAttr = glGetUniformLocation(render::currentShader, "specular");
		int modelAttr = glGetUniformLocation(render::currentShader, "model");
		glUniform1i(diffuseAttr, 0);
		glUniform1i(specularAttr, 1);
		glUniformMatrix4fv(modelAttr, 1, false, (float*)&model);

		boxRenderer.render(render::currentShader);

		shadowRenderer.render(shadowShader.ID, getLightMatrix(normalize(sunDirection)));
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// Lighting pass
		render::start_lighting();


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for (int i = 0; i < pointLights.count; i++)
		{
			pointLightShader.use();
			pointLightShader.setInt("gPosition", 0);
			pointLightShader.setInt("gNormal", 1);
			pointLightShader.setInt("gAlbedoSpec", 2);
			pointLightShader.setVec3("viewPos", camera.Position);
			vec3 position;
			position = transform::positions[transform::indexOf(pointLights.transformIds[i])];
			pointLightShader.setVec3("light.Position", position);
			pointLightShader.setVec3("light.Color", vec3(1, 1, 1));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		directionalLightShader.use();
		directionalLightShader.setInt("gPosition", 0);
		directionalLightShader.setInt("gNormal", 1);
		directionalLightShader.setInt("gAlbedoSpec", 2);
		directionalLightShader.setVec3("viewPos", camera.Position);
		directionalLightShader.setVec3("light.Direction", normalize(sunDirection));
		directionalLightShader.setVec3("light.Color", vec3(1, 1, 1));

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowRenderer.depthMap);
		directionalLightShader.setInt("shadowMap", 4);
		directionalLightShader.setMat4("lightMatrix", getLightMatrix(normalize(sunDirection)));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);


		render::render_deferred();

		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setMat4("model", model);
		lampShader.setVec3("viewPos", camera.Position);
		//lampRenderer.render(lampShader.ID);

		static unsigned int displayTexture = render::finalTexture;
		render::render_screen(displayTexture);
		if (showDebug)
		{
			if(ImGui::Begin("Debug Window", &showDebug));
			{
				ImGui::Checkbox("Pause :", &pauseGame);

				if (ImGui::BeginTabBar("Test Tab Bar"))
				{
					if (ImGui::BeginTabItem("Anchored Rotation"))
					{
						rotation::anchor::showDebug();
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Rotation Animation"))
					{
						rotation::animation::showDebug();
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Cube Movement"))
					{
						ImGui::DragFloat3("Cube Input", (float*)&movement::cube::cubeInput[cubeMovementId.id], 0.1f);
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Render Textures"))
					{
						char buffer[500];
						sprintf_s(buffer, "%d", displayTexture);
						if (ImGui::BeginCombo("Texture", buffer))
						{
							render::debug_texture_selector(&displayTexture);
							if (ImGui::Selectable("Shadow"))
								displayTexture = shadowRenderer.depthMap;
							ImGui::EndCombo();
						}
						ImGui::Image((ImTextureID)displayTexture, ImVec2(400, 300));

						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				
				ImGui::Checkbox("Show Debug", &showDebug);

				static bool showDemoWindow = false;
				ImGui::Checkbox("Show Demo Window", &showDemoWindow);
				
				if(showDemoWindow)
					ImGui::ShowDemoWindow(&showDemoWindow);
			
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