#pragma once

#include "core.h"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

enum eFileType
{
	FILE_TYPE_MAYA = 0,
	FILE_TYPE_3DSMAX,
	FILE_TYPE_BLENDER,
};

struct sFace
{
	int v_id[3];	// vertex indices
	int vt_id[3];	// texCoord indices
	int vn_id[3];	// normals indices
	int mtl_id;		// material index
};

class cVec3f
{
public:

	cVec3f() {}
	cVec3f(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}
	cVec3f operator +(const cVec3f v)
	{
		cVec3f out;
		out.x = this->x + v.x;
		out.y = this->y + v.y;
		out.z = this->z + v.z;

		return out;
	}

	void normalize()
	{
		float d = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);

		this->x /= d;
		this->y /= d;
		this->z /= d;
	}

	cVec3f operator /(const cVec3f v)
	{
		cVec3f out;
		out.x = this->x / v.x;
		out.y = this->y / v.y;
		out.z = this->z / v.z;

		return out;
	}

	float x, y, z;
};


struct newmtl
{
	sRGB	diffuse;
	char	tex[64];
	char	name[64];
};

class cFace
{
public:
	cVec3f	m_v[3];
	cVec3f	m_fn;
};

class cVertex
{
public:
	cVec3f	m_v;  // vertex
	cVec3f	m_n;  // vertex normal
	sVec2f	m_uv; // texture coordinates
};

class cMesh
{
public:
	cMesh();
	~cMesh();

	void init();
	void gen_terrain();

	void createTerrainVBO();

	void render(class cScene *pScene, class cShaderInfo *pShaderInfo);
	void renderTerrain(cScene* pScene, cShaderInfo* pShaderInfo);

	void load(const char *obj_path, const char* obj_name);
	void importMTL();
	void loadFaces();

	void createFaceNormalList();
	void createFaceList();
	void createEdgeList();

	inline int numFaces()
	{
		return m_f_count;
	}
	inline cVec3f *v(int index)
	{
		return &m_v[index];
	}

	inline newmtl	*matList(int index)
	{
		return &m_mtlList[index];
	}

	cTransform	m_transform;

	FILE		*m_fp;
	FILE		*m_mtl;

	char		m_filename[256];
	char		m_name[256];
	int			m_shaderID;

	/////////////////////////////////////////////////////////////////////////////////////
	// VBO:		Vertex Buffer Object holds an array of data. 
	//			The data contained within the VBO is typically vertex, normal, colour and 
	//			texture coordinate data
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int m_VBO;

	/////////////////////////////////////////////////////////////////////////////////////
	// VAO:		Vertex Array Object holds info about how the data stored within the VBO is  
	//			formatted. The VAO holds a pointer to the start of each vertex attribute - 
	//			i.e. vertex, normal, colour, texture coordinate, along with the number of 
	//			elements for each attribute, (a vertex has 3 elements, a texture 
	//			coordinate has 2 elements), the step size (stride) needed to move to the  
	//			beginning of the next item in the list
	/////////////////////////////////////////////////////////////////////////////////////
	unsigned int m_VAO;

	unsigned int m_tex[32];
	
	char m_mtl_filename[64];
	eFileType m_import_filetype;

	int m_objID;
	int m_v_count;
	int m_vt_count;
	int m_vn_count;
	int m_f_count;
	int m_e_count;
	int m_material_count;
	int m_shdr; // shader index - used to select a shader from shader list

	cVec3f *m_v;	// vertex list
	sVec2f *m_vt;	// uv coord list
	cVec3f *m_vn;	// vertex normal list
	cVec3f* m_fn;	// face normals list
	sFace* m_f;	// indices for each face vertex - i.e. m_f[0].x = m_v, m_f[0].y = m_vt, m_f[0].z = m_vn

	cVec3f* m_eList;		// edge list
	cVertex* m_vList;		// vertex list
	newmtl* m_mtlList;		// material list
};

