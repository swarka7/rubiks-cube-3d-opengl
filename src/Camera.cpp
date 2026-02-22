#include <Camera.h>
#include <RubiksCube.h>
#include <Shader.h>
#include <VertexArray.h>
#include <IndexBuffer.h>

#include <algorithm>
#include <cmath>

static glm::vec3 ScreenToWorld(GLFWwindow* window, const Camera& camera, float depth, double mouseX, double mouseY)
{
    int fbW = 0;
    int fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    if (fbW == 0 || fbH == 0)
    {
        return glm::vec3(0.0f);
    }

    float ndcX = (2.0f * static_cast<float>(mouseX) / static_cast<float>(fbW)) - 1.0f;
    float ndcY = 1.0f - (2.0f * static_cast<float>(mouseY) / static_cast<float>(fbH));
    float ndcZ = (2.0f * depth) - 1.0f;

    glm::vec4 clip(ndcX, ndcY, ndcZ, 1.0f);
    glm::mat4 invVP = glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix());
    glm::vec4 world = invVP * clip;
    if (std::abs(world.w) > 1e-6f)
    {
        world /= world.w;
    }

    return glm::vec3(world);
}

Camera::Camera(int width, int height)
    : m_Width(width), m_Height(height)
{
    m_Position = glm::vec3(0.0f, 0.0f, 10.0f);
    m_Target = glm::vec3(0.0f, 0.0f, 0.0f);
    UpdateView();
}

void Camera::SetOrthographic(float near, float far)
{
    m_Near = near;
    m_Far = far;

    // Rest Projection and View matrices
    m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, near, far);
    UpdateView();
}

void Camera::SetPerspective(float fovDegrees, float aspect, float near, float far)
{
    m_Near = near;
    m_Far = far;
    m_Fov = fovDegrees;
    m_Aspect = aspect;

    const float fovRadians = glm::radians(fovDegrees);
    m_Projection = glm::perspective(fovRadians, m_Aspect, near, far);
    UpdateView();
}

void Camera::RotateView(float yawDegrees, float pitchDegrees)
{
    m_Orientation = glm::normalize(glm::rotate(m_Orientation, glm::radians(yawDegrees), glm::vec3(0.0f, 1.0f, 0.0f)));
    m_Orientation = glm::normalize(glm::rotate(m_Orientation, glm::radians(pitchDegrees), glm::vec3(1.0f, 0.0f, 0.0f)));
    UpdateView();
}

void Camera::UpdateView()
{
    glm::vec3 forward = m_Target - m_Position;
    float dist = glm::length(forward);
    if (!std::isfinite(dist) || dist < 0.001f)
    {
        std::cout << "BAD CAMERA dist=" << dist << std::endl;
        m_Position = glm::vec3(0.0f, 0.0f, 10.0f);
        m_Target = glm::vec3(0.0f, 0.0f, 0.0f);
        forward = m_Target - m_Position;
        dist = glm::length(forward);
    }

    if (dist < m_MinDistance)
    {
        glm::vec3 dir = glm::normalize(forward);
        m_Position = m_Target - dir * m_MinDistance;
    }
    else if (dist > m_MaxDistance)
    {
        glm::vec3 dir = glm::normalize(forward);
        m_Position = m_Target - dir * m_MaxDistance;
    }

    forward = m_Target - m_Position;
    float forwardLen = glm::length(forward);
    if (!std::isfinite(forwardLen) || forwardLen < 0.0001f)
    {
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
    }

    m_Orientation = glm::normalize(forward);

    glm::vec3 up = m_Up;
    if (glm::length(glm::cross(m_Orientation, up)) < 1e-6f)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    m_View = glm::lookAt(m_Position, m_Target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Reset()
{
    m_Position = glm::vec3(0.0f, 0.0f, 10.0f);
    m_Target = glm::vec3(0.0f, 0.0f, 0.0f);
    UpdateView();
}

void Camera::Pan(float deltaX, float deltaY)
{
    glm::vec3 forward = m_Target - m_Position;
    float forwardLen = glm::length(forward);
    if (!std::isfinite(forwardLen) || forwardLen < 0.0001f)
    {
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
    }
    forward = glm::normalize(forward);

    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    if (glm::length(glm::cross(forward, worldUp)) < 1e-6f)
    {
        worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    glm::vec3 delta = (-deltaX * m_PanSpeed) * right + (deltaY * m_PanSpeed) * up;
    m_Position += delta;
    m_Target += delta;
    UpdateView();
}

void Camera::Zoom(float scrollDelta)
{
    glm::vec3 forward = m_Target - m_Position;
    float forwardLen = glm::length(forward);
    if (!std::isfinite(forwardLen) || forwardLen < 0.0001f)
    {
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
    }
    forward = glm::normalize(forward);

    m_Position += scrollDelta * m_ZoomSpeed * forward;
    UpdateView();
}

/////////////////////
// Input Callbacks //
/////////////////////

void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    InputContext* context = (InputContext*) glfwGetWindowUserPointer(window);
    if (!context || !context->camera || !context->rubiks || !context->shader) {
        std::cout << "Warning: InputContext wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
        return;
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_P)
    {
        context->pickingEnabled = !context->pickingEnabled;
        if (!context->pickingEnabled && context->rubiks)
        {
            context->rubiks->ResetAll();
        }
        context->selectedId = -1;
        context->selectedDepth = 1.0f;
        context->draggingTranslate = false;
        context->draggingRotate = false;
        context->hasLastWorld = false;
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_UP:
                std::cout << "UP Pressed" << std::endl;
                break;
            case GLFW_KEY_DOWN:
                std::cout << "DOWN Pressed" << std::endl;
                break;
            case GLFW_KEY_LEFT:
                std::cout << "LEFT Pressed" << std::endl;
                break;
            case GLFW_KEY_RIGHT:
                std::cout << "RIGHT Pressed" << std::endl;
                break;
            default:
                break;
        }
    }

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_SPACE:
                context->rotDir *= -1;
                break;
            case GLFW_KEY_Z:
                context->rotAngle = std::max(90, context->rotAngle / 2);
                break;
            case GLFW_KEY_A:
                context->rotAngle = std::min(180, context->rotAngle * 2);
                break;
            case GLFW_KEY_C:
                context->camera->Reset();
                if (context->rubiks)
                {
                    context->rubiks->ResetAll();
                }
                context->selectedId = -1;
                context->selectedDepth = 1.0f;
                context->draggingTranslate = false;
                context->draggingRotate = false;
                context->hasLastWorld = false;
                break;
            default:
                break;
        }
    }

    if (action == GLFW_PRESS && context->rubiks)
    {
        if (context->pickingEnabled)
        {
            return;
        }

        switch (key)
        {
            case GLFW_KEY_R:
                context->rubiks->StartRotation(RubiksCube::Face::Right, context->rotDir, static_cast<float>(context->rotAngle));
                break;
            case GLFW_KEY_L:
                context->rubiks->StartRotation(RubiksCube::Face::Left, context->rotDir, static_cast<float>(context->rotAngle));
                break;
            case GLFW_KEY_U:
                context->rubiks->StartRotation(RubiksCube::Face::Up, context->rotDir, static_cast<float>(context->rotAngle));
                break;
            case GLFW_KEY_D:
                context->rubiks->StartRotation(RubiksCube::Face::Down, context->rotDir, static_cast<float>(context->rotAngle));
                break;
            case GLFW_KEY_F:
                context->rubiks->StartRotation(RubiksCube::Face::Front, context->rotDir, static_cast<float>(context->rotAngle));
                break;
            case GLFW_KEY_B:
                context->rubiks->StartRotation(RubiksCube::Face::Back, context->rotDir, static_cast<float>(context->rotAngle));
                break;
            default:
                break;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    InputContext* context = (InputContext*) glfwGetWindowUserPointer(window);
    if (!context || !context->camera || !context->rubiks || !context->shader || !context->va || !context->ib) {
        std::cout << "Warning: InputContext wasn't set as the Window User Pointer! MouseButtonCallback is skipped" << std::endl;
        return;
    }

    if (action == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &context->camera->m_OldMouseX, &context->camera->m_OldMouseY);
        context->camera->m_NewMouseX = 0.0;
        context->camera->m_NewMouseY = 0.0;
    }

    if (context->pickingEnabled && action == GLFW_PRESS && context->rubiks && context->shader && context->va && context->ib)
    {
        if (context->rubiks->IsRotating())
        {
            return;
        }

        double mouseX = 0.0;
        double mouseY = 0.0;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int fbW = 0;
        int fbH = 0;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        int readX = static_cast<int>(std::clamp(mouseX, 0.0, std::max(0, fbW - 1) * 1.0));
        int readY = fbH - 1 - static_cast<int>(std::clamp(mouseY, 0.0, std::max(0, fbH - 1) * 1.0));

        context->shader->Bind();
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        glm::mat4 view = context->camera->GetViewMatrix();
        glm::mat4 proj = context->camera->GetProjectionMatrix();
        context->rubiks->DrawPicking(*context->shader, *context->va, *context->ib, view, proj);

        GLCall(glFinish());

        unsigned char pixel[4] = {0, 0, 0, 0};
        float depth = 1.0f;
        GLCall(glReadPixels(readX, readY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel));
        GLCall(glReadPixels(readX, readY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth));

        int id = static_cast<int>(pixel[0]) + (static_cast<int>(pixel[1]) << 8) + (static_cast<int>(pixel[2]) << 16);
        if (depth >= 0.9999f || id < 0 || id >= 27)
        {
            context->selectedId = -1;
        }
        else
        {
            context->selectedId = id;
        }

        context->selectedDepth = depth;
        context->draggingTranslate = (button == GLFW_MOUSE_BUTTON_RIGHT && context->selectedId != -1);
        context->draggingRotate = (button == GLFW_MOUSE_BUTTON_LEFT && context->selectedId != -1);
        context->hasLastWorld = false;

        if (context->draggingTranslate && context->selectedId != -1)
        {
            context->lastWorld = ScreenToWorld(window, *context->camera, context->selectedDepth, mouseX, mouseY);
            context->hasLastWorld = true;
        }
        return;
    }

    if (action == GLFW_RELEASE)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            context->draggingRotate = false;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            context->draggingTranslate = false;
        }
        context->hasLastWorld = false;
    }
}

void CursorPosCallback(GLFWwindow* window, double currMouseX, double currMouseY)
{
    InputContext* context = (InputContext*) glfwGetWindowUserPointer(window);
    if (!context || !context->camera || !context->rubiks || !context->shader) {
        std::cout << "Warning: InputContext wasn't set as the Window User Pointer! CursorPosCallback is skipped" << std::endl;
        return;
    }

    double deltaX = currMouseX - context->camera->m_OldMouseX;
    double deltaY = currMouseY - context->camera->m_OldMouseY;

    context->camera->m_NewMouseX = deltaX;
    context->camera->m_NewMouseY = deltaY;
    context->camera->m_OldMouseX = currMouseX;
    context->camera->m_OldMouseY = currMouseY;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (context->pickingEnabled && context->selectedId != -1 && context->draggingRotate && context->rubiks)
        {
            const float rotateSpeed = 0.3f;
            glm::vec3 forward = glm::normalize(context->camera->GetTarget() - context->camera->GetPosition());
            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 up = glm::normalize(glm::cross(right, forward));

            context->rubiks->ApplyPickRotation(context->selectedId, right, static_cast<float>(deltaY) * rotateSpeed);
            context->rubiks->ApplyPickRotation(context->selectedId, up, static_cast<float>(deltaX) * rotateSpeed);
        }
        else if (!context->pickingEnabled && context->rubiks)
        {
            const float sensitivity = 0.2f;
            context->rubiks->AddGlobalRotation(static_cast<float>(deltaX) * sensitivity, static_cast<float>(deltaY) * sensitivity);
        }
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (context->pickingEnabled && context->selectedId != -1 && context->draggingTranslate && context->rubiks)
        {
            double mouseX = currMouseX;
            double mouseY = currMouseY;
            glm::vec3 world = ScreenToWorld(window, *context->camera, context->selectedDepth, mouseX, mouseY);

            if (context->hasLastWorld)
            {
                glm::vec3 deltaWorld = world - context->lastWorld;
                context->rubiks->ApplyPickTranslation(context->selectedId, deltaWorld);
            }

            context->lastWorld = world;
            context->hasLastWorld = true;
        }
        else if (!context->pickingEnabled)
        {
            context->camera->Pan(static_cast<float>(deltaX), static_cast<float>(deltaY));
        }
    }
}

void ScrollCallback(GLFWwindow* window, double scrollOffsetX, double scrollOffsetY)
{
    InputContext* context = (InputContext*) glfwGetWindowUserPointer(window);
    if (!context || !context->camera || !context->rubiks || !context->shader) {
        std::cout << "Warning: InputContext wasn't set as the Window User Pointer! ScrollCallback is skipped" << std::endl;
        return;
    }
    context->camera->Zoom(static_cast<float>(scrollOffsetY));
}

void Camera::EnableInputs(GLFWwindow* window, InputContext* context)
{
    // Set camera as the user pointer for the window
    glfwSetWindowUserPointer(window, context);

    // Handle key inputs
    glfwSetKeyCallback(window, (void(*)(GLFWwindow *, int, int, int, int)) KeyCallback);

    // Handle cursor buttons
    glfwSetMouseButtonCallback(window, (void(*)(GLFWwindow *, int, int, int)) MouseButtonCallback);

    // Handle cursor position and inputs on motion
    glfwSetCursorPosCallback(window , (void(*)(GLFWwindow *, double, double)) CursorPosCallback);

    // Handle scroll inputs
    glfwSetScrollCallback(window, (void(*)(GLFWwindow *, double, double)) ScrollCallback);
}
