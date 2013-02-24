#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <fstream>
using namespace std;
#include "texture.h"


class Model {
private:
	struct VertexType {
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct ModelType {
		float x,y,z;
		float tu, tv;
		float nx,ny,nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType {
		float x, y, z;
	};

	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture* m_texture;
	ID3D10ShaderResourceView* textures[2];
	Texture* m_textureBump;
	ModelType* m_modelType;
	float m_positionX, m_positionY, m_positionZ;

	bool InitBuffers(ID3D10Device*);
	void ShutdownBuffers(void);
	void RenderBuffers(ID3D10Device*);
	bool LoadTexture(ID3D10Device*, WCHAR*);
	
	void ReleaseTexture(void);
	bool LoadModel(char*);
	void ReleaseModel(void);
	//Bumpspecific
	
	void CalculateModelVectors(void);
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

public:
	Model(void);
	~Model(void);
	bool LoadBumpTexture(ID3D10Device*, WCHAR*);
	ID3D10ShaderResourceView** GetTextureArray(void);
	bool Init(ID3D10Device*, char*, WCHAR*);
	void Shutdown(void);
	void Render(ID3D10Device*);
	int GetIndexCount(void) { return m_indexCount; }
	ID3D10ShaderResourceView* GetTexture(void) { return m_texture->GetTexture(); }
	void SetPosition(float,float,float);
	void GetPosition(float&,float&,float&);
};


#endif