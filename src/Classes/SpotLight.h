#ifndef _Light_H_
#define _Light_H_

#include <d3dx10math.h>
#include "ILight.h"

//This light supports having their own view and projection matrices
class SpotLight : public ILight
{
public:
	SpotLight();
	SpotLight(const SpotLight&);
	~SpotLight();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);
	
	//new in Exercise 3 for Bump Mapping
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);
	void SetDirection(float, float, float);


	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetPosition();

	//new for bump mapping:
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();
	D3DXVECTOR3 GetDirection();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(D3DXMATRIX&);
	void GetProjectionMatrix(D3DXMATRIX&);

private:
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAt;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;

	//new for bump mapping:
	D3DXVECTOR4 m_specularColor;
	float m_specularPower;
	D3DXVECTOR3 m_direction;
};

#endif