
class Camera;

namespace render
{
	extern unsigned int currentShader;
	extern unsigned int finalTexture;

	void init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);
	void clear_frame();
	void start_render(const Camera& camera);
	void start_lighting();
	void render_screen(int texture);
	void render_deferred();

	void debug_texture_selector(unsigned int* selectedTexture);
}