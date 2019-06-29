
class Camera;

namespace render
{
	namespace shaders
	{
		namespace gBuffer
		{
			extern GLuint shader;
			extern GLuint diffuseAttr;
			extern GLuint specularAttr;
			extern GLuint modelAttr;
			extern GLuint projectionAttr;
			extern GLuint viewAttr;
			extern GLuint viewPosAttr;
		}
	}

	extern unsigned int finalTexture;

	void init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);
	void clear_frame();
	void start_render();
	void start_lighting();
	void render_screen(int texture);
	void render_deferred();

	void debug_texture_selector(unsigned int* selectedTexture);
}