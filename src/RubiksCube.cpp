#include <RubiksCube.h>

#include <Debugger.h>
#include <IndexBuffer.h>
#include <Shader.h>
#include <VertexArray.h>

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace
{
    constexpr int FacePosX = 0;
    constexpr int FaceNegX = 1;
    constexpr int FacePosY = 2;
    constexpr int FaceNegY = 3;
    constexpr int FacePosZ = 4;
    constexpr int FaceNegZ = 5;
}

RubiksCube::RubiksCube(float spacing, const glm::vec3& baseOffset)
    : m_BaseOffset(baseOffset), m_Spacing(spacing)
{
    InitSolvedLayout();
    CaptureInitialState();
}

void RubiksCube::InitSolvedLayout()
{
    m_Cubelets.clear();
    m_Cubelets.reserve(27);
    m_Active = ActiveRotation();

    int id = 0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            for (int z = -1; z <= 1; ++z)
            {
                Cubelet cubelet;
                cubelet.id = id;
                cubelet.idx = glm::ivec3(x, y, z);
                cubelet.orient = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                cubelet.pickOffset = glm::vec3(0.0f);
                cubelet.faceColors.fill(glm::vec3(0.0f));
                cubelet.model = glm::mat4(1.0f);

                m_PosToId[x + 1][y + 1][z + 1] = id;
                m_IdToPos[id] = cubelet.idx;

                if (x == 1)
                {
                    cubelet.faceColors[FacePosX] = glm::vec3(0.8f, 0.1f, 0.1f);
                }
                else if (x == -1)
                {
                    cubelet.faceColors[FaceNegX] = glm::vec3(0.9f, 0.4f, 0.1f);
                }

                if (y == 1)
                {
                    cubelet.faceColors[FacePosY] = glm::vec3(0.95f, 0.95f, 0.95f);
                }
                else if (y == -1)
                {
                    cubelet.faceColors[FaceNegY] = glm::vec3(0.95f, 0.85f, 0.1f);
                }

                if (z == 1)
                {
                    cubelet.faceColors[FacePosZ] = glm::vec3(0.1f, 0.3f, 0.85f);
                }
                else if (z == -1)
                {
                    cubelet.faceColors[FaceNegZ] = glm::vec3(0.1f, 0.7f, 0.2f);
                }

                m_Cubelets.push_back(cubelet);
                ++id;
            }
        }
    }

    UpdateModels();
}

void RubiksCube::CaptureInitialState()
{
    for (int id = 0; id < 27; ++id)
    {
        m_InitialIdToPos[id] = m_IdToPos[id];
        m_InitialOrient[id] = m_Cubelets[id].orient;
    }
    m_HasInitialState = true;
}

void RubiksCube::Reset()
{
    InitSolvedLayout();
}

void RubiksCube::ResetAll()
{
    ASSERT(m_HasInitialState);
    m_GlobalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    m_Active = ActiveRotation();

    for (int id = 0; id < 27; ++id)
    {
        m_IdToPos[id] = m_InitialIdToPos[id];
        m_Cubelets[id].idx = m_InitialIdToPos[id];
        m_Cubelets[id].orient = m_InitialOrient[id];
        m_Cubelets[id].pickOffset = glm::vec3(0.0f);
    }

    for (int id = 0; id < 27; ++id)
    {
        const glm::ivec3& idx = m_IdToPos[id];
        m_PosToId[idx.x + 1][idx.y + 1][idx.z + 1] = id;
    }

    UpdateModels();
}

void RubiksCube::ResetGlobalRotation()
{
    m_GlobalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

void RubiksCube::Update(float deltaSeconds)
{
    if (!m_Active.active)
    {
        return;
    }

    m_Active.currentAngle += m_Active.speed * deltaSeconds;
    if (m_Active.currentAngle >= m_Active.targetAngle)
    {
        m_Active.currentAngle = m_Active.targetAngle;
        ApplyRotation();
        m_Active.active = false;
        m_Active.mask.fill(false);
    }
}

bool RubiksCube::StartRotation(Face face, int direction, float angleDegrees)
{
    if (m_Active.active)
    {
        return false;
    }

    m_Active = ActiveRotation();
    m_Active.active = true;
    m_Active.targetAngle = angleDegrees;
    m_Active.currentAngle = 0.0f;

    glm::ivec3 axis(0);
    int layer = 0;
    int clockwiseSign = 1;

    switch (face)
    {
        case Face::Right:
            axis = glm::ivec3(1, 0, 0);
            layer = 1;
            clockwiseSign = -1;
            break;
        case Face::Left:
            axis = glm::ivec3(1, 0, 0);
            layer = -1;
            clockwiseSign = 1;
            break;
        case Face::Up:
            axis = glm::ivec3(0, 1, 0);
            layer = 1;
            clockwiseSign = -1;
            break;
        case Face::Down:
            axis = glm::ivec3(0, 1, 0);
            layer = -1;
            clockwiseSign = 1;
            break;
        case Face::Front:
            axis = glm::ivec3(0, 0, 1);
            layer = 1;
            clockwiseSign = -1;
            break;
        case Face::Back:
            axis = glm::ivec3(0, 0, 1);
            layer = -1;
            clockwiseSign = 1;
            break;
    }

    m_Active.axis = axis;
    m_Active.layer = layer;
    m_Active.direction = direction * clockwiseSign;

    for (int id = 0; id < 27; ++id)
    {
        const glm::ivec3& idx = m_IdToPos[id];
        if ((axis.x != 0 && idx.x == layer) ||
            (axis.y != 0 && idx.y == layer) ||
            (axis.z != 0 && idx.z == layer))
        {
            m_Active.mask[id] = true;
        }
    }

    return true;
}

void RubiksCube::UpdateModels()
{
    for (auto& cubelet : m_Cubelets)
    {
        glm::vec3 pos = glm::vec3(cubelet.idx) * m_Spacing + cubelet.pickOffset;
        glm::mat4 rot = glm::mat4_cast(cubelet.orient);
        glm::mat4 scl = glm::scale(glm::mat4(1.0f), glm::vec3(m_CubeletScale));
        cubelet.model = glm::translate(glm::mat4(1.0f), pos) * rot * scl;
    }
}

void RubiksCube::AddGlobalRotation(float yawDegrees, float pitchDegrees)
{
    glm::quat yaw = glm::angleAxis(glm::radians(yawDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat pitch = glm::angleAxis(glm::radians(pitchDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
    m_GlobalRotation = glm::normalize(yaw * pitch * m_GlobalRotation);
}

void RubiksCube::ApplyPickTranslation(int id, const glm::vec3& deltaWorld)
{
    if (id < 0 || id >= static_cast<int>(m_Cubelets.size()))
    {
        return;
    }

    glm::mat3 invGlobal = glm::mat3_cast(glm::inverse(m_GlobalRotation));
    glm::vec3 deltaLocal = invGlobal * deltaWorld;
    m_Cubelets[id].pickOffset += deltaLocal;
}

void RubiksCube::ApplyPickRotation(int id, const glm::vec3& axisWorld, float angleDegrees)
{
    if (id < 0 || id >= static_cast<int>(m_Cubelets.size()))
    {
        return;
    }

    glm::mat3 invGlobal = glm::mat3_cast(glm::inverse(m_GlobalRotation));
    glm::vec3 axisLocal = glm::normalize(invGlobal * axisWorld);
    glm::quat rot = glm::angleAxis(glm::radians(angleDegrees), axisLocal);
    m_Cubelets[id].orient = glm::normalize(rot * m_Cubelets[id].orient);
}

void RubiksCube::ApplyRotation()
{
    int steps = static_cast<int>(std::round(m_Active.targetAngle / 90.0f));
    if (steps < 1)
    {
        steps = 1;
    }
    if (steps > 2)
    {
        steps = 2;
    }

    glm::vec3 axisVec = glm::normalize(glm::vec3(m_Active.axis));
    glm::quat rot = glm::angleAxis(glm::radians(m_Active.direction * m_Active.targetAngle), axisVec);

    for (int id = 0; id < 27; ++id)
    {
        if (!m_Active.mask[id])
        {
            continue;
        }

        glm::ivec3 idx = m_IdToPos[id];
        for (int i = 0; i < steps; ++i)
        {
            idx = RotateIndex90(idx, m_Active.axis, m_Active.direction);
        }

        m_IdToPos[id] = idx;
        m_Cubelets[id].idx = idx;
        m_Cubelets[id].orient = rot * m_Cubelets[id].orient;
    }

    for (int id = 0; id < 27; ++id)
    {
        const glm::ivec3& idx = m_IdToPos[id];
        m_PosToId[idx.x + 1][idx.y + 1][idx.z + 1] = id;
    }
}

glm::ivec3 RubiksCube::RotateIndex90(const glm::ivec3& idx, const glm::ivec3& axis, int direction) const
{
    glm::ivec3 out = idx;
    if (axis.x != 0)
    {
        if (direction > 0)
        {
            out.y = -idx.z;
            out.z = idx.y;
        }
        else
        {
            out.y = idx.z;
            out.z = -idx.y;
        }
    }
    else if (axis.y != 0)
    {
        if (direction > 0)
        {
            out.x = idx.z;
            out.z = -idx.x;
        }
        else
        {
            out.x = -idx.z;
            out.z = idx.x;
        }
    }
    else if (axis.z != 0)
    {
        if (direction > 0)
        {
            out.x = -idx.y;
            out.y = idx.x;
        }
        else
        {
            out.x = idx.y;
            out.y = -idx.x;
        }
    }
    return out;
}

void RubiksCube::Draw(Shader& shader, VertexArray& va, IndexBuffer& ib, const glm::mat4& view, const glm::mat4& proj)
{
    UpdateModels();

    va.Bind();
    ib.Bind();

    glm::mat4 global = glm::translate(glm::mat4(1.0f), m_BaseOffset) * glm::mat4_cast(m_GlobalRotation);
    glm::mat4 anim = glm::mat4(1.0f);
    bool useAnim = m_Active.active;
    if (useAnim)
    {
        glm::vec3 axisVec = glm::normalize(glm::vec3(m_Active.axis));
        float signedAngle = m_Active.direction * m_Active.currentAngle;
        anim = glm::mat4_cast(glm::angleAxis(glm::radians(signedAngle), axisVec));
    }

    for (const auto& cubelet : m_Cubelets)
    {
        glm::mat4 local = cubelet.model;
        if (useAnim && m_Active.mask[cubelet.id])
        {
            local = anim * local;
        }

        glm::mat4 model = global * local;
        glm::mat4 mvp = proj * view * model;

        shader.SetUniform3f("u_FaceColors[0]", cubelet.faceColors[FacePosX]);
        shader.SetUniform3f("u_FaceColors[1]", cubelet.faceColors[FaceNegX]);
        shader.SetUniform3f("u_FaceColors[2]", cubelet.faceColors[FacePosY]);
        shader.SetUniform3f("u_FaceColors[3]", cubelet.faceColors[FaceNegY]);
        shader.SetUniform3f("u_FaceColors[4]", cubelet.faceColors[FacePosZ]);
        shader.SetUniform3f("u_FaceColors[5]", cubelet.faceColors[FaceNegZ]);

        shader.SetUniformMat4f("u_Model", model);
        shader.SetUniformMat4f("u_MVP", mvp);
        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
    }
}

void RubiksCube::DrawPicking(Shader& shader, VertexArray& va, IndexBuffer& ib, const glm::mat4& view, const glm::mat4& proj)
{
    UpdateModels();

    va.Bind();
    ib.Bind();

    glm::mat4 global = glm::translate(glm::mat4(1.0f), m_BaseOffset) * glm::mat4_cast(m_GlobalRotation);
    glm::mat4 anim = glm::mat4(1.0f);
    bool useAnim = m_Active.active;
    if (useAnim)
    {
        glm::vec3 axisVec = glm::normalize(glm::vec3(m_Active.axis));
        float signedAngle = m_Active.direction * m_Active.currentAngle;
        anim = glm::mat4_cast(glm::angleAxis(glm::radians(signedAngle), axisVec));
    }

    shader.SetUniform1i("u_Picking", 1);

    for (const auto& cubelet : m_Cubelets)
    {
        glm::mat4 local = cubelet.model;
        if (useAnim && m_Active.mask[cubelet.id])
        {
            local = anim * local;
        }

        glm::mat4 mvp = proj * view * (global * local);

        int id = cubelet.id;
        float r = static_cast<float>(id & 0xFF) / 255.0f;
        float g = static_cast<float>((id >> 8) & 0xFF) / 255.0f;
        float b = static_cast<float>((id >> 16) & 0xFF) / 255.0f;
        glm::vec4 pickColor(r, g, b, 1.0f);

        shader.SetUniform4f("u_Color", pickColor);
        shader.SetUniformMat4f("u_Model", global * local);
        shader.SetUniformMat4f("u_MVP", mvp);
        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
    }
}
