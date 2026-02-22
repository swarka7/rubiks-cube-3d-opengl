#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Debugger.h>
class Shader;
class VertexArray;
class IndexBuffer;
class RubiksCube;
struct InputContext;

class Camera
{
    private:
        // View and Projection
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);

        // View matrix paramters
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 10.0f);
        glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 m_Target = glm::vec3(0.0f, 0.0f, 0.0f);

        float m_PanSpeed = 0.01f;
        float m_ZoomSpeed = 0.5f;
        float m_MinDistance = 2.0f;
        float m_MaxDistance = 50.0f;

        // Projection matrix parameters
        float m_Near = 0.1f; 
        float m_Far = 100.0f;
        float m_Fov = 45.0f;
        float m_Aspect = 1.0f;
        int m_Width;
        int m_Height;

        // Orthographic Projection parameters
        float m_Left = -1.0f;
        float m_Right = 1.0f;
        float m_Bottom = -1.0f; 
        float m_Top = 1.0f;
    public:
        // Prevent the camera from jumping around when first clicking left click
        double m_OldMouseX = 0.0;
        double m_OldMouseY = 0.0;
        double m_NewMouseX = 0.0;
        double m_NewMouseY = 0.0;
    public:
        Camera(int width, int height);

        // Update Projection matrix for Orthographic mode
        void SetOrthographic(float near, float far);
        void SetPerspective(float fovDegrees, float aspect, float near, float far);

        void RotateView(float yawDegrees, float pitchDegrees);
        void UpdateView();
        void Reset();
        void Pan(float deltaX, float deltaY);
        void Zoom(float scrollDelta);

        // Handle camera inputs
        void EnableInputs(GLFWwindow* window, InputContext* context);

        inline glm::mat4 GetViewMatrix() const { return m_View; }
        inline glm::mat4 GetProjectionMatrix() const { return m_Projection; }
        inline const glm::vec3& GetPosition() const { return m_Position; }
        inline const glm::vec3& GetTarget() const { return m_Target; }
};

struct InputContext
{
    Camera* camera = nullptr;
    RubiksCube* rubiks = nullptr;
    Shader* shader = nullptr;
    VertexArray* va = nullptr;
    IndexBuffer* ib = nullptr;
    int rotDir = 1;
    int rotAngle = 90;
    bool pickingEnabled = false;
    int selectedId = -1;
    float selectedDepth = 1.0f;
    bool draggingTranslate = false;
    bool draggingRotate = false;
    glm::vec3 lastWorld = glm::vec3(0.0f);
    bool hasLastWorld = false;
};
