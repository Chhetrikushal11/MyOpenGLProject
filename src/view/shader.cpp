#include "view/shader.h"

unsigned int make_shader(
    const std::string& vertex_filepath, const std::string& fragment_filepath)
{
    std::cout << "Loading shaders..." << std::endl;
    
    // to store all the shader modules
    std::vector<unsigned int> modules;
    
    // add a vertex shader module
    unsigned int vertModule = make_module(vertex_filepath, GL_VERTEX_SHADER);
    if (vertModule == 0) {
        std::cerr << "Failed to create vertex shader module!" << std::endl;
        return 0;
    }
    modules.push_back(vertModule);
    
    // add a fragment shader module
    unsigned int fragModule = make_module(fragment_filepath, GL_FRAGMENT_SHADER);
    if (fragModule == 0) {
        std::cerr << "Failed to create fragment shader module!" << std::endl;
        return 0;
    }
    modules.push_back(fragModule);
    
    // attach all the modules then link the program
    unsigned int shader = glCreateProgram();
    for (unsigned int shaderModule : modules) {
        glAttachShader(shader, shaderModule);
    }
    glLinkProgram(shader);
    
    // check the linking worked
    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        char errorLog[1024];
        glGetProgramInfoLog(shader, 1024, NULL, errorLog);
        std::cout << "Shader linking error: \n" << errorLog << '\n';
        return 0;
    } else {
        std::cout << "Shaders linked successfully! Program ID: " << shader << std::endl;
    }
    
    // modules are now unneeded and can be freed
    for (unsigned int shaderModule : modules) {
        glDeleteShader(shaderModule);
    }
    
    return shader;
}

unsigned int make_module(const std::string filepath, unsigned int module_type)
{
    std::ifstream file;
    std::stringstream bufferedLines;
    std::string line;
    
    file.open(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open shader file: " << filepath << std::endl;
        return 0;
    }
    
    std::cout << "Reading shader file: " << filepath << std::endl;
    
    while (std::getline(file, line)) {
        bufferedLines << line << '\n';
    }
    
    std::string shaderSource = bufferedLines.str();
    const char* shaderSrc = shaderSource.c_str();
    file.close();
    
    std::cout << "Shader source length: " << shaderSource.length() << " characters" << std::endl;
    
    unsigned int shaderModule = glCreateShader(module_type);
    glShaderSource(shaderModule, 1, &shaderSrc, NULL);
    glCompileShader(shaderModule);
    
    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errorLog[1024];
        glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
        std::cout << "Shader Module compilation error:\n" << errorLog << std::endl;
        return 0;
    } else {
        const char* typeStr = (module_type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        std::cout << typeStr << " shader compiled successfully! Module ID: " << shaderModule << std::endl;
    }
    
    return shaderModule;
}