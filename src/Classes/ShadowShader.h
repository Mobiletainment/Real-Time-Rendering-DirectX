#ifndef _ShadowShader_H_
#define _ShadowShader_H_

#include <fstream>
#include "IShadowShader.h"

using namespace std;

class ShadowShader : public IShadowShader
{
public:
	ShadowShader();
	ShadowShader(const ShadowShader&);
	~ShadowShader();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, 
		ID3D10ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

	//Interface method: With Texture Array (for bump mapping):
	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, ID3D10ShaderResourceView** texture, 
		ID3D10ShaderResourceView* depthMapTexture, D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, ID3D10ShaderResourceView*,
		D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
	void RenderShader(ID3D10Device*, int);

private:
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

	//new in V2: We have two new pointers for the two new light matrices. 
	ID3D10EffectMatrixVariable* m_lightViewMatrixPtr;
	ID3D10EffectMatrixVariable* m_lightProjectionMatrixPtr;

	ID3D10EffectShaderResourceVariable* m_texturePtr;
	//new in V2: we require a texture for the render to texture of the depth buffer from the light's perspective (shadow map in short). 
	ID3D10EffectShaderResourceVariable* m_depthMapTexturePtr;

	ID3D10EffectVectorVariable* m_lightPositionPtr;
	ID3D10EffectVectorVariable* m_ambientColorPtr;
	ID3D10EffectVectorVariable* m_diffuseColorPtr;
};

#endif