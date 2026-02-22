#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <vector>

class Shader;
class VertexArray;
class IndexBuffer;

struct Cubelet
{
    int id = 0;
    glm::ivec3 idx = glm::ivec3(0);
    glm::quat orient = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 pickOffset = glm::vec3(0.0f);
    std::array<glm::vec3, 6> faceColors{};
    glm::mat4 model = glm::mat4(1.0f);
};

class RubiksCube
{
    public:
        enum class Face
        {
            Right,
            Left,
            Up,
            Down,
            Front,
            Back
        };

        RubiksCube(float spacing, const glm::vec3& baseOffset);

        void Reset();
        void ResetAll();
        void ResetGlobalRotation();
        void Update(float deltaSeconds);
        bool StartRotation(Face face, int direction, float angleDegrees);
        bool IsRotating() const { return m_Active.active; }
        void UpdateModels();
        void AddGlobalRotation(float yawDegrees, float pitchDegrees);
        void ApplyPickTranslation(int id, const glm::vec3& deltaWorld);
        void ApplyPickRotation(int id, const glm::vec3& axisWorld, float angleDegrees);
        void Draw(Shader& shader, VertexArray& va, IndexBuffer& ib, const glm::mat4& view, const glm::mat4& proj);
        void DrawPicking(Shader& shader, VertexArray& va, IndexBuffer& ib, const glm::mat4& view, const glm::mat4& proj);

        const std::vector<Cubelet>& GetCubelets() const { return m_Cubelets; }
        const glm::ivec3& GetCubeletPos(int id) const { return m_IdToPos[id]; }
        int GetCubeletId(const glm::ivec3& idx) const { return m_PosToId[idx.x + 1][idx.y + 1][idx.z + 1]; }

    private:
        struct ActiveRotation
        {
            bool active = false;
            glm::ivec3 axis = glm::ivec3(0);
            int layer = 0;
            int direction = 1;
            float targetAngle = 0.0f;
            float currentAngle = 0.0f;
            float speed = 180.0f;
            std::array<bool, 27> mask = {};
        };

        void ApplyRotation();
        glm::ivec3 RotateIndex90(const glm::ivec3& idx, const glm::ivec3& axis, int direction) const;

        glm::vec3 m_BaseOffset = glm::vec3(0.0f);
        float m_Spacing = 1.0f;
        float m_CubeletScale = 0.95f;
        glm::quat m_GlobalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        ActiveRotation m_Active;

        void InitSolvedLayout();
        void CaptureInitialState();

        std::vector<Cubelet> m_Cubelets;
        int m_PosToId[3][3][3] = {};
        glm::ivec3 m_IdToPos[27];
        std::array<glm::ivec3, 27> m_InitialIdToPos{};
        std::array<glm::quat, 27> m_InitialOrient{};
        bool m_HasInitialState = false;
};
