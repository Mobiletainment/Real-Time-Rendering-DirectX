#include "SpotLight.h"

SpotLight::SpotLight()
{
}

SpotLight::SpotLight(const SpotLight& other)
{
}

SpotLight::~SpotLight()
{
}


void SpotLight::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void SpotLight::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void SpotLight::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void SpotLight::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}


void SpotLight::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}

void SpotLight::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR3(x, y, z);
	return;
}

//The SetLookAt function sets the m_lookAt vector so that we can set where the light is looking at. This vector is used to build the light's view matrix. 
void SpotLight::SetLookAt(float x, float y, float z)
{
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
	return;
}

D3DXVECTOR4 SpotLight::GetAmbientColor()
{
	return m_ambientColor;
}

D3DXVECTOR4 SpotLight::GetDiffuseColor()
{
	return m_diffuseColor;
}

D3DXVECTOR3 SpotLight::GetPosition()
{
	return m_position;
}



D3DXVECTOR4 SpotLight::GetSpecularColor()
{
	return m_specularColor;
}


float SpotLight::GetSpecularPower()
{
	return m_specularPower;
}


D3DXVECTOR3 SpotLight::GetDirection()
{
	return m_direction;
}
//The view matrix for the light is built using the up vector, the lookAt vector, and the position of the light. 
void SpotLight::GenerateViewMatrix()
{
	D3DXVECTOR3 up;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Create the view matrix from the three vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_lookAt, &up);

	return;
}

//The projection matrix for the light is built using the field of view, viewing aspect ratio, and the near and far plane of the light range.
//The light we are projecting is more of a square spotlight than a true point light, but this is necessary since we need to align with the sampling from a square shadow map texture.
//That is why the field of view and aspect ratio are setup for a square projection. 
void SpotLight::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;


	// Setup field of view and screen aspect for a square light source.
	//Öffnungswinkel = 90°
	fieldOfView = (float)D3DX_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	return;
}

//We also have two new functions to return the view and projection matrices. 
void SpotLight::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void SpotLight::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}