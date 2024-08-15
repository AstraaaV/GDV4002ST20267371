#include "core.h"
#include "cPlayer.h"
#include "cMesh.h"

/////////////////////////////////////////////////////////////////////////////////////
// update() - 
/////////////////////////////////////////////////////////////////////////////////////
void cScene::update()
{
	m_Camera[m_cameraId].update();

	for (int i = 0; i < m_mesh_count; i++)
	{
		m_Mesh[i].m_objID = i;
		m_Mesh[i].m_transform.update();
	}

	//update camera
	followTarget();

	//update aim to match pos of current player
	m_Mesh[Aim].m_transform.m_pos.x = m_Mesh[currentPlayer].m_transform.m_pos.x;
	m_Mesh[Aim].m_transform.m_pos.y = m_Mesh[currentPlayer].m_transform.m_pos.y;

	//if firing is true, apply gravity to boulder
	if (!firing)
	{
		boulderGravity();
	}
}

void cScene::genTerrain()
{
	m_Mesh[m_groundId].gen_terrain();

	vec3 v1, v2;
	vec3 z = vec3(0.0, 0.0, 1.0);
	vec3 terrainPos;

	// loop through all mesh objects and find the ground mesh..
	for (int j = 0; j < m_mesh_count; j++)
	{
		if (strcmp(m_Mesh[j].m_name, "ground") == 0)
		{
			// we are here... that means counter j must be the ground mesh ID. Let's store this value here
			int m_groundId = j;

			// loop through all vertices in the ground mesh
			for (int i = 0; i < m_Mesh[m_groundId].m_v_count - 1; i++)
			{
				// Steps needed to calculate the NORMAL for a single line segment on the terrian mesh...
				vec3 v;

				// Get the start and end vertices for the line segment we want to use - i.e. m_Mesh[m_groundId].m_vList[i] and m_Mesh[m_groundId].m_vList[i + 1]
				v1.x = m_Mesh[m_groundId].m_vList[i].m_v.x;
				v1.y = m_Mesh[m_groundId].m_vList[i].m_v.y;
				v1.z = m_Mesh[m_groundId].m_vList[i].m_v.z;

				v2.x = m_Mesh[m_groundId].m_vList[i + 1].m_v.x;
				v2.y = m_Mesh[m_groundId].m_vList[i + 1].m_v.y;
				v2.z = m_Mesh[m_groundId].m_vList[i + 1].m_v.z;
				
				// calculate the vector between these two vertices (i.e. v = end-start)
				// normalize v so that it is of unit length
				v = v2 - v1;
				normalize(v);

				// calculate the CROSS product between v and z
				vec3 n = cross(v, z);

				// update m_Mesh[m_groundId].m_vList[i].m_n with this newly calculated xyz vector
				m_Mesh[m_groundId].m_vList[i].m_n.x = n.x;
				m_Mesh[m_groundId].m_vList[i].m_n.y = n.y;
				m_Mesh[m_groundId].m_vList[i].m_n.z = n.z;
			}
			terrainPos = m_Mesh[m_groundId].m_transform.m_pos;
		}
	}

	m_Mesh[m_groundId].createTerrainVBO();
}

bool cScene::isBoulderHit()
{
	//if current player is player one (2nd torus)
	if (currentPlayer == PlayerOne)
	{
		if (m_Mesh[Boulder].m_transform.m_pos.x <= m_Mesh[PlayerTwo].m_transform.m_pos.x + m_Mesh[PlayerOne].m_transform.m_scale.x
			&& m_Mesh[Boulder].m_transform.m_pos.x >= m_Mesh[PlayerTwo].m_transform.m_pos.x - m_Mesh[PlayerOne].m_transform.m_scale.x)
		{
			return true; //if boulder hits player one return true
		}
	}
	return false;  //if no hit detected
}

void cScene::goRight()
{
	//player movement to the right
	m_Mesh[currentPlayer].m_transform.m_pos.x += 0.05f;
}

void cScene::goLeft()
{
	//player movement to the left
	m_Mesh[currentPlayer].m_transform.m_pos.x -= 0.05f;
}

void cScene::followTarget()
{
	//camera follows current player position
	m_Camera->m_pos.x = m_Mesh[currentPlayer].m_transform.m_pos.x;
	m_Camera->m_pos.y = m_Mesh[currentPlayer].m_transform.m_pos.y;

	//light follows current player position
	m_Light->m_pos.x = m_Mesh[currentPlayer].m_transform.m_pos.x;
	m_Light->m_pos.y = m_Mesh[currentPlayer].m_transform.m_pos.y;
}

void cScene::hitRight()
{
	//aim right
	m_Mesh[Aim].m_transform.m_rot.z -= 0.5f;
}

void cScene::hitLeft()
{
	//aim left
	m_Mesh[Aim].m_transform.m_rot.z += 0.5f;
}

void cScene::fire()
{
	//only allow firing if currentplayer is player one (2nd torus)
	if (currentPlayer == PlayerOne)
	{
		if (!firing)
		{
			// Firing is true
			firing = true;

			//set boulder pos to player pos
			m_Mesh[Boulder].m_transform.m_pos.x = m_Mesh[PlayerOne].m_transform.m_pos.x;
			m_Mesh[Boulder].m_transform.m_pos.y = m_Mesh[PlayerOne].m_transform.m_pos.y;

			//set boulder vel according to rotation
			m_Mesh[Boulder].m_transform.m_vel.x = -m_Mesh[Aim].m_transform.m_rot.z * boulderSpeed;
			m_Mesh[Boulder].m_transform.m_vel.y = m_Mesh[Aim].m_transform.m_scale.y * boulderSpeed;

			//set gravity
			gravity = 0.01f;
			//amount of bounces
			boulderMovement = 3;

			fireTimer = fireCoolDown; //start cooldown for firing

			//if boulder is active
			boulderRendered = true;
			boulderTimer = 0.0f;
		}
	}
}

bool cScene::getFire()
{
	return firing; //return current state of firing variable
}

//gravity for boulder
void cScene::boulderGravity()
{
	float d;

	//boulder's velocity (Vertical) based on gravity
	m_Mesh[Boulder].m_transform.m_vel.y -= gravity;

	// find the line segment that this mesh is positioned over
	for (int i = 0; i < m_Mesh[m_groundId].m_v_count - 1; i++)
	{
		// perform a containment check between either end of the line segment
		if (m_Mesh[m_groundId].m_vList[i].m_v.x <= m_Mesh[Boulder].m_transform.m_pos.x && m_Mesh[m_groundId].m_vList[i + 1].m_v.x >= m_Mesh[Boulder].m_transform.m_pos.x)
		{
			vec3 a = vec3(m_Mesh[m_groundId].m_vList[i].m_v.x + m_Mesh[m_groundId].m_transform.m_pos.x,
				m_Mesh[m_groundId].m_vList[i].m_v.y + m_Mesh[m_groundId].m_transform.m_pos.y,
				m_Mesh[m_groundId].m_vList[i].m_v.z + m_Mesh[m_groundId].m_transform.m_pos.z);
			vec3 c = m_Mesh[Boulder].m_transform.m_pos;

			// drop mesh down onto this line
			// a (x – x1) + b (y– y1) + c (z –z1) = 0

			d = (m_Mesh[m_groundId].m_vList[i].m_n.x * (a.x - (c.x))) +
				(m_Mesh[m_groundId].m_vList[i].m_n.y * (a.y - (c.y))) +
				(m_Mesh[m_groundId].m_vList[i].m_n.z * (a.z - (c.z)));
			
			if (d < 0.0) //checks if boulder is below terrain
			{
				//if so, moves boulder upwards
				m_Mesh[Boulder].m_transform.m_pos.y -= d * 0.1f;

				//decreases bounce counter
				boulderMovement -= 1;

				//reflected velocity of boulder
				vec3 boulderReflect;
				//current velocity of boulder
				vec3 boulderVel = m_Mesh[Boulder].m_transform.m_vel;
				vec3 surfaceNormal; //holds surface normal

				//sets the surface normal using ground mesh vertex normal
				surfaceNormal.x = m_Mesh[m_groundId].m_vList[i].m_n.x;
				surfaceNormal.y = m_Mesh[m_groundId].m_vList[i].m_n.y;
				surfaceNormal.z = m_Mesh[m_groundId].m_vList[i].m_n.z;

				//calc reflection of boulder using surface normal and boulder vel
				boulderReflect = boulderVel - 2.0f * dot(surfaceNormal, boulderVel) * surfaceNormal;
				boulderReflect.y *= -1.0f; //inverts y axis

				//updated boulder vel to new reflect vel
				m_Mesh[Boulder].m_transform.m_vel = boulderReflect;

				//if boulder has hit player
				if (isBoulderHit())
				{
					//point system depending on player
					if (currentPlayer == PlayerOne)
					{
						POnePoints += 1;
					}
					else if (currentPlayer == PlayerTwo)
					{
						PTwoPoints += 1;
					}
				}

				//firing set to false if no more boulder movements
				if (boulderMovement == 0)
				{
					firing = false;
				}
			}
		}
	}
}

void cScene::selectPlayer(int playerSelection)
{
	//switching between players
	if (playerSelection == 1)
	{
		currentPlayer = PlayerTwo;
	}
	else if (playerSelection == 2)
	{
		currentPlayer = PlayerOne;
	}
}

void cScene::drop2Ground()
{
	float d;

	vec3 terrainPos = m_Mesh[m_groundId].m_transform.m_pos;

	// loop through all mesh objects again..
	for (int j = 0; j < m_mesh_count; j++)
	{
		// ignore the ground object 
		if (strcmp(m_Mesh[j].m_name, "ground") == 0)
			continue;

		// find the line segment that this mesh is positioned over
		for (int i = 0; i < m_Mesh[m_groundId].m_v_count-1; i++)
		{
			// perform a containment check between either end of the line segment
			if (m_Mesh[m_groundId].m_vList[i].m_v.x <= m_Mesh[j].m_transform.m_pos.x && m_Mesh[m_groundId].m_vList[i + 1].m_v.x >= m_Mesh[j].m_transform.m_pos.x)
			{
				vec3 a = vec3(m_Mesh[m_groundId].m_vList[i].m_v.x + m_Mesh[m_groundId].m_transform.m_pos.x,
					m_Mesh[m_groundId].m_vList[i].m_v.y + m_Mesh[m_groundId].m_transform.m_pos.y,
					m_Mesh[m_groundId].m_vList[i].m_v.z + m_Mesh[m_groundId].m_transform.m_pos.z);
				vec3 c = m_Mesh[j].m_transform.m_pos;

				 
				// drop mesh down onto this line
				// a (x – x1) + b (y– y1) + c (z –z1) = 0

				d = (m_Mesh[m_groundId].m_vList[i].m_n.x * (a.x - (c.x))) +
					(m_Mesh[m_groundId].m_vList[i].m_n.y * (a.y - (c.y))) +
					(m_Mesh[m_groundId].m_vList[i].m_n.z * (a.z - (c.z)));
				//if (d != 0.0f)
					m_Mesh[j].m_transform.m_pos.y -= d*0.1f;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// init() - 
/////////////////////////////////////////////////////////////////////////////////////
void cScene::init()
{
	openSDF();

	POnePoints = 0; //set p1 to zero points at start of game
	PTwoPoints = 0; //set p2 to zero points at start of game

	// initialize the meshes
	for (int i = 0; i < m_mesh_count; i++)
	{
		m_Mesh[i].init();

		if (strcmp(m_Mesh[i].m_name, "ground") == 0)
			m_groundId = i;
	}
	
	//place p1 to ground level
	m_Mesh[PlayerOne].m_transform.m_pos.y = 0.0f;
	//place p2 to ground level
	m_Mesh[PlayerTwo].m_transform.m_pos.y = 0.0f;

	genTerrain();

	// initialize the camera
	m_Camera->init();

	glBindVertexArray(0);

	// create the shaders
	m_ShaderInfo.create();
}

/////////////////////////////////////////////////////////////////////////////////////
// render() - 
/////////////////////////////////////////////////////////////////////////////////////
void cScene::render()
{
	//if game has started
	if(m_start)
		//call drop to ground function
		drop2Ground();

	// update the scene
	update();

	// loop through and render each mesh object in this scene
	for (int i = 0; i < m_mesh_count; i++)
	{
		//checks if current index is for boulder
		if (i == Boulder && boulderRendered)
			//render boulder mesh
			m_Mesh[i].render(this, &m_ShaderInfo);
		//if current index is not for boulder
		else if(i != Boulder)
			//render other objects
			m_Mesh[i].render(this, &m_ShaderInfo);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// openSDF() - Opens up a scene descriptor file
/////////////////////////////////////////////////////////////////////////////////////
void cScene::openSDF()
{
	// load the scene file here

	FILE* fp = NULL;
	
	fopen_s(&fp, "scene.sdf", "r");

	if (!fp)
		assert(0);

	char buffer[256] = { "\0"};
	float value;

	// init all counters to zero
	m_light_count = 0;
	m_mesh_count = 0;
	m_shader_count = 0;

	char* pch;

	// loop through and get the number of assets we have in the scene
	do
	{
		fgets(buffer, 255, fp);
		pch = strstr(buffer, "LIGHT:");
		if (pch != NULL) m_light_count++;

		pch = strstr(buffer, "MESH:");
		if (pch != NULL) m_mesh_count++;

		pch = strstr(buffer, "SHADER:");
		if (pch != NULL) m_shader_count++;

	} while (!feof(fp));

	rewind(fp);
	
	////////////////////////////////////////////////////////////
	// CAMERA
	////////////////////////////////////////////////////////////
	
	// allocate block of memory to store the camera
	m_Camera = new cCamera;

	fscanf_s(fp, "%s%s", buffer, 256, buffer, 256); // CAMERA {
	fscanf_s(fp, "%s%f", buffer, 256, &m_Camera->m_fov);		// FOV:	50.0
	fscanf_s(fp, "%s%f", buffer, 256, &m_Camera->m_near);		// NEAR:	0.5
	fscanf_s(fp, "%s%f", buffer, 256, &m_Camera->m_far);		// FAR:	60.0
	fscanf_s(fp, "%s%f%f%f", buffer, 256, &m_Camera->m_pos.x, &m_Camera->m_pos.y, &m_Camera->m_pos.z);		// POS:		0.0 0.0 10.0
	fscanf_s(fp, "%s", buffer, 256);				// }
	
	m_Camera->m_target_pos = m_Camera->m_pos;


	////////////////////////////////////////////////////////////
	// LIGHTS
	////////////////////////////////////////////////////////////
	
	// allocate block of memory to store the light..
	m_Light = new cLight[m_light_count];

	for (int i = 0; i < m_light_count; i++)
	{
		fscanf_s(fp, "%s%s", buffer, 256, buffer, 256);	// LIGHT: }
		m_Light[i].m_type = 0; // diffuse
		fscanf_s(fp, "%s%f%f%f", buffer, 256, &m_Light[i].m_pos.x, &m_Light[i].m_pos.y, &m_Light[i].m_pos.z); // POS:	-25.0 0.0 20.0
		fscanf_s(fp, "%s", buffer, 256);				// }
	}

	////////////////////////////////////////////////////////////
	// MESHES
	////////////////////////////////////////////////////////////
	char filename[256];
	char name[256];

	// allocate block of memory to store the mesh objects..
	m_Mesh = new cMesh[m_mesh_count];

	for (int i = 0; i < m_mesh_count; i++)
	{
		fscanf_s(fp, "%s%s", buffer, 256, m_Mesh[i].m_filename, 256);	//MESH: torus.obj
		fscanf_s(fp, "%s", buffer, 256);					// }
		fscanf_s(fp, "%s%s", buffer, 256, m_Mesh[i].m_name, 256);		//MESH: torus.obj
		fscanf_s(fp, "%s%f%f%f", buffer, 256, &m_Mesh[i].m_transform.m_pos.x, &m_Mesh[i].m_transform.m_pos.y, &m_Mesh[i].m_transform.m_pos.z);					// POS:	0.0 0.0 0.0
		fscanf_s(fp, "%s%f%f%f", buffer, 256, &m_Mesh[i].m_transform.m_rot.x, &m_Mesh[i].m_transform.m_rot.y, &m_Mesh[i].m_transform.m_rot.z);					// ROT : 2.0 2.0 0.0
		fscanf_s(fp, "%s%f%f%f", buffer, 256, &m_Mesh[i].m_transform.m_rot_incr.x, &m_Mesh[i].m_transform.m_rot_incr.y, &m_Mesh[i].m_transform.m_rot_incr.z);	// ROT_INCR:	1.0 1.0 0.0
		fscanf_s(fp, "%s%f%f%f", buffer, 256, &m_Mesh[i].m_transform.m_scale.x, &m_Mesh[i].m_transform.m_scale.y, &m_Mesh[i].m_transform.m_scale.z);			// SCALE : 1.25 1.25 1.25
		fscanf_s(fp, "%s%f%f%f", buffer, 256, &m_Mesh[i].m_transform.m_vel.x, &m_Mesh[i].m_transform.m_vel.y, &m_Mesh[i].m_transform.m_vel.z);					// VEL : 0.2 0.1 0.0
		fscanf_s(fp, "%s%d", buffer, 256, &m_Mesh[i].m_shaderID); // SHADER_ID	0

		fscanf_s(fp, "%s", buffer, 256);					// }
	}

	////////////////////////////////////////////////////////////
	// SHADERS
	////////////////////////////////////////////////////////////
	ShaderTable = new sShaderDetails[m_shader_count];
	
	fscanf_s(fp, "%s", buffer, 256); // SHADER:

	for (int i = 0; i < m_shader_count; i++)
	{
		// init shader types..
		for( int j=0; j<6; j++ )
			ShaderTable[i].types[j] = 0;


		fscanf_s(fp, "%s", buffer, 256);		
		strcpy_s(ShaderTable[i].filename, buffer);
		
		int loop_count = 0;
		
		while (strcmp(buffer, "SHADER:") != 0)
		{
			loop_count++;

			fscanf_s(fp, "%s", buffer, 256);
			if (strcmp(buffer, "SHADER:") == 0)
				break;

			// sort the shader by type and add to list..
			if (strcmp(buffer, "VERT_SHDR") == 0)
				ShaderTable[i].types[0] = GL_VERTEX_SHADER;
			else
			if (strcmp(buffer, "CTRL_SHDR") == 0)
				ShaderTable[i].types[1] = GL_TESS_CONTROL_SHADER;
			else
			if (strcmp(buffer, "EVAL_SHDR") == 0)
				ShaderTable[i].types[2] = GL_TESS_EVALUATION_SHADER;
			else
			if (strcmp(buffer, "GEOM_SHDR") == 0)
				ShaderTable[i].types[3] = GL_GEOMETRY_SHADER;
			else
			if (strcmp(buffer, "FRAG_SHDR") == 0)
				ShaderTable[i].types[4] = GL_FRAGMENT_SHADER;
			else
				ShaderTable[i].types[5] = GL_COMPUTE_SHADER;

			if (loop_count == 6)
				break;
		};
	}

	fclose(fp);

}