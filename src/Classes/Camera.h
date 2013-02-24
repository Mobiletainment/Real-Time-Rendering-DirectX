#ifndef _Camera_H_
#define _Camera_H_

#include <d3dx10math.h>
#include <vector>
#include <iostream>
#include <sstream>

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void MoveCamera(float forward, float sideways);
	void Initialize(float x, float y, float z);
	void YawPitchRollCamera(float yawDeg, float pitchDeg, float rollDeg);
	void Render();

	void CameraJourneyWithInterpolation( D3DXVECTOR3 &vPosition, D3DXQUATERNION &qRotation );
	void GetViewMatrix(D3DXMATRIX&);
	void SetAxis(D3DXVECTOR3 &vLookAt, D3DXVECTOR3 &vUp, D3DXVECTOR3 &vRight );
	void SetFocalPointAndPosition(const D3DXVECTOR3 focalPoint, const D3DXVECTOR3 upVec, const D3DXVECTOR3 position);
	void SavePoint();
	void RunPoints();
	void SetSyncTime(float syncTime);
	void GetDirection(D3DXMATRIX &camRotation);
	D3DXQUATERNION m_qRotation;

private:
	static const D3DXVECTOR3 WORLDAXIS_X;
	static const D3DXVECTOR3 WORLDAXIS_Y;
	static const D3DXVECTOR3 WORLDAXIS_Z;
	static const float SPEED;

	struct point
	{
		D3DXVECTOR3 position;
		D3DXQUATERNION rotation;
	};

	D3DXMATRIX m_viewMatrix;

	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAt;

	std::vector<point> m_savedPoints;
	point m_destinationPoint;
	point m_startPoint;
	point m_farPoint;
	bool m_isAutoDriving;
	int m_pointsCounter;
	float m_updateTime;
	float m_progress;
	float m_totalYawDeg;
	float m_totalPitchDeg;
	float m_totalRollDeg;
};

#endif