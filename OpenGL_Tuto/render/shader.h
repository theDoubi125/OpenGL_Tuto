
namespace shader
{
	void checkCompileErrors(unsigned int shader, std::string type);

	unsigned int compileShader(const char* vertexPath, const char* fragmentPath);
};