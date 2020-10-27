#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#ifdef _WIN32
#include <stb_image.h>
#elif __linux__
#include <stb/stb_image.h>
#endif

#include <fstream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool GLLogCall(const char *function, const char *file, int line)
{
    if (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << std::endl
                  << file << ": " << line << std::endl;
        return false;
    }
    return true;
}

#define ASSERT(x) if (!(x))
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

bool filExists(const char *p_file)
{
    std::ifstream inputFile;
    inputFile.open(p_file, std::ifstream::in);
    return inputFile.fail() ? false : true;
}

int main(int argc, char const **argv)
{
    if (argc != 2)
    {
        if (argc == 1)
        {
            std::cout << "Error: No input\n";
            return EXIT_FAILURE;
        }
        else
        {
            std::cout << "Error: Too many inputs\n";
            return EXIT_FAILURE;
        }
    }

    if (!filExists(argv[1]))
    {
        std::cout << "File does not exist or cannot be open\n";
        return EXIT_FAILURE;
    }

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

    unsigned int VBO;
    GLCall(glGenBuffers(1, &VBO));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    unsigned int VAO;
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glBindVertexArray(VAO));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, 0, 5 * sizeof(float), 0));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, 0, 5 * sizeof(float), (void *)(3 * sizeof(float))));
    GLCall(glEnableVertexAttribArray(1));

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

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *win, int x, int y) {
        GLCall(glViewport(0, 0, x, y));
        GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
        glfwSwapBuffers(win);
    });

    glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);
        glfwPollEvents();
    });

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        int x, y;
        glfwGetWindowSize(window, &x, &y);
        glViewport(0, 0, x, y);
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
