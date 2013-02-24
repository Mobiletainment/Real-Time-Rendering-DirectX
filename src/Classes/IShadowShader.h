#pragma once
#ifndef ISHADOWSHADER_H
#define ISHADOWSHADER_H

#include <d3d10.h>
#include <d3dx10math.h>

class IShadowShader
{
public:
	virtual void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, ID3D10ShaderResourceView** texture, 
		ID3D10ShaderResourceView* depthMapTexture, D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor) = 0;
};

#endif