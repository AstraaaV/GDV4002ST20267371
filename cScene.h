#pragma once

#include "core.h"

class cPlayer;
class cMesh;

class cScene
{
public:

	cScene() 
	{
		m_start = false;
		m_cameraId	= 0;
		m_lightId	= 0;
	}

	~cScene() {}
	void init();
	void render();
	void update();
	void openSDF();

	void drop2Ground();
	void genTerrain();
	bool isBoulderHit();

	bool firing = false;

	void goRight();
	void goLeft();

	void followTarget();
	void hitRight();
	void hitLeft();

	void fire();
	float boulderSpeed = 0.001f;
	bool getFire();

	float fireCoolDown = 0.1f;
	float fireTimer = 0.0f;

	void boulderGravity();
	bool boulderRendered = false;

	float boulderTimer = 0.0f;
	float boulderLifespan = 0.1f;

	float gravity;
	int boulderMovement;
	int POnePoints;
	int PTwoPoints;

	cShaderInfo		m_ShaderInfo;
	cCamera*		m_Camera;
	cLight*			m_Light;
	cMesh*			m_Mesh;
	cPlayer*		m_Player;

	sShaderDetails* ShaderTable;

	bool m_start;
	int m_shader_count;
	int m_light_count;
	int m_mesh_count;
	int m_cameraId;
	int m_lightId;
	int m_groundId;
	int m_turn = false;
	float result;

	enum meshes{PlayerOne = 0, PlayerTwo, Boulder, Floor, Aim};
	int currentPlayer;
	void selectPlayer(int playerSelection);

private:

};