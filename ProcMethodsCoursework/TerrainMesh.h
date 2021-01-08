////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>
#include <stdlib.h>
#include "SimplexNoise.h"
#include "../DXFramework/BaseMesh.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainMesh : public BaseMesh
{
private:

	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	TerrainMesh(ID3D11Device*, int terrainWidth, int terrainHeight);
	TerrainMesh(const TerrainMesh&);
	~TerrainMesh();

	bool Initialize(ID3D11Device*, char*);
	bool InitializeTerrain(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool GenerateNoise(ID3D11Device* device, bool keydown);
	bool FaultTerrain(ID3D11Device* device, bool keydown);
	int  GetIndexCount();
	float RandomiseTerrain(int i, int j);
	bool SmoothTerrain(ID3D11Device* device, bool keydown);

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void initBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool inBounds(UINT i, UINT j);

private:
	bool m_terrainGeneratedToggle;
	bool m_terrainSmoothedToggle;
	int m_terrainWidth, m_terrainHeight;
	//int vertexCount, indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	HeightMapType* m_heightMap;

	SimplexNoise* simNoise;

	float displacement = 2.5f;
};

#endif
