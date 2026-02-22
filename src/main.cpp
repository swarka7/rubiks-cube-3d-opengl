#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <Camera.h>
#include <RubiksCube.h>

#include <iostream>
#include <cstdio>

/* Window size */
const unsigned int width = 800;
const unsigned int height = 800;
// const float FOVdegree = 45.0f;  // Field Of View Angle
const float near = 0.1f;
const float far = 100.0f;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    GLCall(glViewport(0, 0, width, height));

    InputContext* context = (InputContext*) glfwGetWindowUserPointer(window);
    if (!context || !context->camera)
    {
        return;
    }

    float aspect = (height == 0) ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
    context->camera->SetPerspective(45.0f, aspect, near, far);
}

/* Cube vertices coordinates with positions, colors, texCoords, and normals */
float vertices[] = {
    // positions            // colors            // texCoords      // normals
    // Front (+Z)
    -0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    // Back (-Z)
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
    // Left (-X)
    -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,    -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,    -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,    -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,    -1.0f, 0.0f, 0.0f,
    // Right (+X)
     0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
    // Top (+Y)
    -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
    // Bottom (-Y)
    -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f, -1.0f, 0.0f,
};

/* Indices for vertices order */
unsigned int indices[] = {
    0, 1, 2,  2, 3, 0,       // Front
    4, 5, 6,  6, 7, 4,       // Back
    8, 9, 10, 10, 11, 8,     // Left
    12, 13, 14, 14, 15, 12,  // Right
    16, 17, 18, 18, 19, 16,  // Top
    20, 21, 22, 22, 23, 20   // Bottom
};

int main(int argc, char* argv[])
{
    GLFWwindow* window;

    glfwSetErrorCallback([](int code, const char* desc) {
        printf("GLFW ERROR %d: %s\n", code, desc);
        fflush(stdout);
    });

    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }
    
    /* Set OpenGL to Version 3.3.0 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    /* Load GLAD so it configures OpenGL */
    gladLoadGL();

    /* Control frame rate */
    glfwSwapInterval(1);

    /* Print OpenGL version after completing initialization */
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Controls: R/L/U/D/F/B rotate walls, Space flip direction, Z/A 90/180, "
                 "Left-drag rotate cube, Right-drag pan, Scroll zoom, P picking, C reset"
              << std::endl;

    /* Set scope so that on widow close the destructors will be called automatically */
    {
        int fbW = 0;
        int fbH = 0;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        GLCall(glViewport(0, 0, fbW, fbH));

        /* Blend to fix images with transperancy */
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        /* Generate VAO, VBO, EBO and bind them */
        VertexArray va;
        VertexBuffer vb(vertices, sizeof(vertices));
        IndexBuffer ib(indices, sizeof(indices));

        VertexBufferLayout layout;
        layout.Push<float>(3);  // positions
        layout.Push<float>(3);  // colors
        layout.Push<float>(2);  // texCoords
        layout.Push<float>(3);  // normals
        va.AddBuffer(vb, layout);

        /* Create texture */
        Texture texture("res/textures/plane.png");
        texture.Bind();
         
        /* Create shaders */
        Shader shader("res/shaders/basic.shader");
        shader.Bind();

        /* Unbind all to prevent accidentally modifying them */
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        /* Enables the Depth Buffer */
    	GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDepthFunc(GL_LESS));
        GLCall(glDisable(GL_CULL_FACE));

        /* Create camera */
        Camera camera(width, height);
        float aspect = (fbH == 0) ? 1.0f : static_cast<float>(fbW) / static_cast<float>(fbH);
        camera.SetPerspective(45.0f, aspect, near, far);

        RubiksCube rubiks(1.05f, glm::vec3(0.0f));
        static InputContext inputContext;
        inputContext.camera = &camera;
        inputContext.rubiks = &rubiks;
        inputContext.shader = &shader;
        inputContext.va = &va;
        inputContext.ib = &ib;
        camera.EnableInputs(window, &inputContext);
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
        std::cout << "Startup mode: FOV=45, angle=" << inputContext.rotAngle
                  << ", dir=" << (inputContext.rotDir > 0 ? "CW" : "CCW")
                  << ", picking=" << (inputContext.pickingEnabled ? "ON" : "OFF")
                  << ", selectedId=" << inputContext.selectedId
                  << std::endl;

        float lastFrameTime = static_cast<float>(glfwGetTime());

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            float currentTime = static_cast<float>(glfwGetTime());
            float deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;

            rubiks.Update(deltaTime);

            /* Set white background color */
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            /* Initialize uniform color */
            glm::vec4 color = glm::vec4(1.0, 1.0f, 1.0f, 1.0f);

            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 proj = camera.GetProjectionMatrix();

            /* Update shaders paramters and draw to the screen */
            shader.Bind();
            shader.SetUniform4f("u_Color", color);
            shader.SetUniform1i("u_Picking", 0);
            shader.SetUniform1i("u_Texture", 0);
            rubiks.Draw(shader, va, ib, view, proj);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
