#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "shader_m.h"
#include "camera.h"

#include <iostream>

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
#include "gameplay/world/voxel.h"

#include "scene/scene.h"

#include "gameplay/input.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool showDebug = true;
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
	glfwSetCursorPosCallback(window, input::mouseMovement);
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
	glEnable(GL_CULL_FACE);

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

	input::window = window;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	

	render::init(SCR_WIDTH, SCR_HEIGHT);
	scene::init(SCR_WIDTH, SCR_HEIGHT);
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

		if (!showDebug)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		}

		// view/projection transformations fixed for all renderers
		

		if (pauseGame)
		{
			deltaTime = 0;
		}

		scene::update(deltaTime);

		

		scene::render();

		// Lighting pass


		
		//lampRenderer.render(lampShader.ID);

		static unsigned int displayTexture = render::finalTexture;

		render::render_screen(displayTexture);

		char buffer[500];
		sprintf_s(buffer, "%d", displayTexture);
		if (ImGui::BeginCombo("Texture", buffer))
		{
			render::debug_texture_selector(&displayTexture);
			/*if (ImGui::Selectable("Shadow"))
				displayTexture = shadowRenderer.depthMap;*/
			ImGui::EndCombo();
		}
		ImGui::Image((ImTextureID)displayTexture, ImVec2(400, 300));
		scene::showDebug();
		if (showDebug)
		{
		/*	if(ImGui::Begin("Debug Window", &showDebug));
			{
				ImGui::Checkbox("Pause :", &pauseGame);

				if (ImGui::BeginTabBar("Test Tab Bar"))
				{
					if (ImGui::BeginTabItem("Transform"))
					{
						
						ImGui::EndTabItem();

					}
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
					if (ImGui::BeginTabItem("Voxel"))
					{
						voxel::chunkEditor(chunk, meshLibrary);
						ImGui::EndTabItem();

					}
					if (ImGui::BeginTabItem("Render Textures"))
					{
						

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Shadows"))
					{
						debug::displayElement("sun direction", sunDirection);
						debug::displayElement("sun pov distance", sunPovDistance);
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
			}*/
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
	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//mainCamera.ProcessMouseScroll(yoffset);
}