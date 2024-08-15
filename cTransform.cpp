#include "core.h"
#include "cScene.h"

using namespace glm;


//////////////////////////////////////////////////////////////////////////////////////////
// update() - 
//////////////////////////////////////////////////////////////////////////////////////////
void cTransform::update()
{
    m_rot += m_rot_incr;
    m_pos += m_vel;

    /*if (m_pos.x > 2.65 || m_pos.x < -2.65)
    {
        m_vel.x = -m_vel.x;
    }

    if (m_pos.y > 1.95 || m_pos.y < -1.95)
    {
        m_vel.y = -m_vel.y;
    }*/

    m_ModelMatrix = glm::translate(mat4(1.0), vec3(m_pos));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(m_scale));
}

//reverse x axis of velocity
void cTransform::reverseX()
{
    m_vel.x *= -1;
}

//reverse y axis of velocity
void cTransform::reverseY()
{
    m_vel.y *= -1;
}

//reverse z axis of velocity
void cTransform::reverseZ()
{
    m_vel.z *= -1;
}

//reverse all components of velocity
void cTransform::reverseXYZ()
{
    m_vel *= -1;
}

