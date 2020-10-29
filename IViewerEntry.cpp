#include "checks/argChecker.hpp"
#include "texture/texture.hpp"
#include "checks/fileChecker.hpp"
#include "util.hpp"

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#ifdef _WIN32
#include <stb_image.h>
#elif __linux__
#include <stb/stb_image.h>
#endif

#include <fstream>

int main(int argc, char const **argv)
{
    //Check if an image was passed to the program
    if (IViewer::checkArgs(argc, 1, true))
        return EXIT_FAILURE;

    //Check if file exists
    if (!IViewer::filExists(argv[1]))
    {
        std::cout << "File does not exist or cannot be open\n";
        return EXIT_FAILURE;
    }

    //Get image data and meta data
    IViewer::Texture tex(argv[0]);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *imageData = stbi_load(argv[1], &width, &height, &nrChannels, 0);
    if (!imageData)
    {
        std::cout << "----------------------\n";
        std::cout << "Failed to laod texture!\n";
        std::cout << stbi_failure_reason() << std::endl;
        std::cout << "----------------------\n";
    }
    glfwInit();
    std::cout << "Width: " << width << std::endl
              << "Height: " << height << std::endl
              << "Color Channels: " << nrChannels << std::endl;
    GLFWwindow *window = glfwCreateWindow(width, height, " ", NULL, NULL);
    glfwMakeContextCurrent(window);
    unsigned int err = glewInit();
    if (err)
    {
        std::cout << glewGetErrorString(err) << std::endl;
        return EXIT_FAILURE;
    }

    //OpenGL code
    float vertices[] = {
        -1, 1, 0, 0, 1,  //0
        1, 1, 0, 1, 1,   //1
        -1, -1, 0, 0, 0, //2
        1, 1, 0, 1, 1,
        1, -1, 0, 1, 0,
        -1, -1, 0, 0, 0 //3
    };

    //VBO
    unsigned int VBO;
    GLCall(glGenBuffers(1, &VBO));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    //VAO
    unsigned int VAO;
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glBindVertexArray(VAO));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, 0, 5 * sizeof(float), 0));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, 0, 5 * sizeof(float), (void *)(3 * sizeof(float))));
    GLCall(glEnableVertexAttribArray(1));

    //Texture
    unsigned int imageTexture;
    GLCall(glGenTextures(1, &imageTexture));

    GLCall(glBindTexture(GL_TEXTURE_2D, imageTexture));
    GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    if (nrChannels == 3)
    {
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                            0, GL_RGB, GL_UNSIGNED_BYTE, imageData));
    }
    else if (nrChannels == 4)
    {
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                            0, GL_RGBA, GL_UNSIGNED_BYTE, imageData));
    }
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));

    stbi_image_free(imageData);

    const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 texCoord;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
        "   texCoord = aTexCoord;\n"
        "}\0";

    const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 texCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(ourTexture, texCoord);\n"
        "} \n";

    //Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER), fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLCall(glShaderSource(vertexShader, 1, &vertexShaderSource, NULL));
    GLCall(glCompileShader(vertexShader));
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    GLCall(glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL));
    GLCall(glCompileShader(fragmentShader));
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    //Shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    GLCall(glAttachShader(shaderProgram, vertexShader));
    GLCall(glAttachShader(shaderProgram, fragmentShader));
    GLCall(glLinkProgram(shaderProgram));

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING\n"
                  << infoLog << std::endl;
    }

    //Set the framebuffer callback, for drawing while resizing
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *win, int x, int y) {
        GLCall(glViewport(0, 0, x, y));
        GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
        glfwSwapBuffers(win);
    });
    //Set the key callback, to process keyboard input
    glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);
        glfwPollEvents();
    });

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        GLCall(glBindTexture(GL_TEXTURE_2D, imageTexture));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GLCall(glBindVertexArray(VAO));
        GLCall(glUseProgram(shaderProgram));

        GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
