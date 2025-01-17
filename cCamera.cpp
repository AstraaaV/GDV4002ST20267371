#include "cCamera.h"


/////////////////////////////////////////////////////////////////////////////////////
// constructor
/////////////////////////////////////////////////////////////////////////////////////
cCamera::cCamera()
{
	projectionMatrix = glm::perspective(glm::radians(m_fov), (float)800.0f / (float)600.0f, m_near, m_far);	
	m_pos   = vec3(0.0f, 0.0f, 7.5f);
	m_vel = vec3(0.005f, 0.0f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////////////
// destructor
/////////////////////////////////////////////////////////////////////////////////////
cCamera::~cCamera()
{
}

/////////////////////////////////////////////////////////////////////////////////////
// init() - 
/////////////////////////////////////////////////////////////////////////////////////
void cCamera::init()
{
	projectionMatrix = glm::perspective(glm::radians(m_fov), (float)800.0f / (float)600.0f, m_near, m_far);
}

/////////////////////////////////////////////////////////////////////////////////////
// update() - 
/////////////////////////////////////////////////////////////////////////////////////
void cCamera::update()
{	
	float d = glm::length(m_target_pos - m_pos);
	if (d >= 0.1f)
		m_pos += m_vel;
	else
	{
		m_pos = m_target_pos;
	}

	mat4 t	= glm::translate(mat4(1.0f), vec3(-m_pos));
	viewMatrix = t;
}