#pragma once
#ifndef cPLAYER_H_
#define cPLAYER_H_

#include "core.h"

class cPlayer
{
public:
	cPlayer();
	~cPlayer();
	cPlayer(cMesh m_Mesh);

	void init();
	void update();
private:
	cMesh* player_Mesh;
	bool touchTerrain;
};
#endif // !cPLAYER_H_


