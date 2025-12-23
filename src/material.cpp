// material.cpp - Updated version with STB implementation and debugging
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "material.h"

Material::Material(const char* filename)
{
    std::cout << "Attempting to load texture: " << filename << std::endl;
    int width, height, channels;

    // Flip image vertically (OpenGL expects origin at bottom-left)
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
    {
        std::cerr << "ERROR: Failed to load texture: " << filename << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        std::cerr << "Make sure the file exists and is a valid image format (JPEG/PNG)" << std::endl;

        // CREATE A FALLBACK 1x1 WHITE TEXTURE
        unsigned char white[4] = { 255, 255, 255, 255 };
        width = 1;
        height = 1;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);

        std::cout << "Created fallback white texture with ID: " << texture << std::endl;
        return;
    }

    std::cout << "Image loaded successfully: " << width << "x" << height
        << " with " << channels << " channels" << std::endl;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free image data
    stbi_image_free(data);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error after texture creation: " << error << std::endl;
    }
    else
    {
        std::cout << "Texture created successfully with ID: " << texture << std::endl;
    }
}

Material::~Material()
{
    if (texture != 0)
    {
        glDeleteTextures(1, &texture);
    }
}

void Material::use(int unit) const
{
    // Now texture is ALWAYS valid (either loaded or fallback white)
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}