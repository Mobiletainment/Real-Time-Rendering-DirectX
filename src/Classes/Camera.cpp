#include "Camera.h"

const D3DXVECTOR3 Camera::WORLDAXIS_Y(0.0f, 1.0f, 0.0f);
const float Camera::SPEED = 0.5f;

Camera::Camera()
{
	m_progress = 0;
	m_updateTime = 0.0f;
	m_pointsCounter = 0;
	m_isAutoDriving = false;
	m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXQuaternionIdentity(&m_qRotation);
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::Initialize(float x, float y, float z)
{
	SetFocalPointAndPosition(D3DXVECTOR3(0.0f, 0.0f, 1.0f), WORLDAXIS_Y, D3DXVECTOR3( 0.0f, 0.0f, -28.0f ));
}

void Camera::SetFocalPointAndPosition(const D3DXVECTOR3 focalPoint, const D3DXVECTOR3 upVec, const D3DXVECTOR3 position)
{
	D3DXVECTOR3 vUp, vRight;

	m_position = position;
	m_lookAt = focalPoint - position;

	D3DXVec3Normalize(&m_lookAt, &m_lookAt);
	D3DXVec3Cross(&vRight, &upVec, &m_lookAt);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Cross(&vUp, &m_lookAt, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	SetAxis(m_lookAt, vUp, vRight);
}

void Camera::SetAxis(D3DXVECTOR3 &vLookAt, D3DXVECTOR3 &vUp, D3DXVECTOR3 &vRight )
{
	D3DXMATRIX mRotation;
	D3DXMatrixIdentity(&mRotation);

	mRotation(0,0) = vRight.x;		mRotation(1,0) = vRight.y;		mRotation(2,0) = vRight.z;
	mRotation(0,1) = vUp.x;			mRotation(1,1) = vUp.y;			mRotation(2,1) = vUp.z;
	mRotation(0,2) = vLookAt.x;		mRotation(1,2) = vLookAt.y;		mRotation(2,2) = vLookAt.z;

	D3DXQuaternionRotationMatrix( &m_qRotation, &mRotation );
	//D3DXQuaternionNormalize(&m_qRotation, &m_qRotation);
}

void Camera::GetDirection(D3DXMATRIX &camRotation)
{
	if(!m_isAutoDriving)
	{
		D3DXMatrixRotationQuaternion( &camRotation, &m_qRotation );
	}

	return;
}

void Camera::MoveCamera(float forward, float sideways)
{
	if(!m_isAutoDriving)
	{
		D3DXMATRIX mRot;
		D3DXVECTOR3 vRight;
		D3DXVECTOR3 vDirection;
		D3DXVECTOR3 vMove;

		D3DXMatrixRotationQuaternion( &mRot, &m_qRotation );

		vRight = D3DXVECTOR3( mRot(0,0), mRot(1,0), mRot(2,0) );
		vDirection = D3DXVECTOR3( mRot(0,2), mRot(1,2), mRot(2,2) );

		vMove = ((vRight * sideways) + (vDirection * forward));
		D3DXVec3Normalize(&vMove, &vMove);
		m_position += vMove;
	}
	
	return;
}

void Camera::YawPitchRollCamera(float yawDeg, float pitchDeg, float rollDeg)
{
	if(!m_isAutoDriving) //don't move camera when auto-pilot is on
	{
		D3DXQUATERNION qRotation;
		D3DXQuaternionIdentity(&qRotation);
		D3DXQuaternionRotationYawPitchRoll(&qRotation, (float) D3DXToRadian(yawDeg), (float) D3DXToRadian(pitchDeg), (float) D3DXToRadian(rollDeg));
		D3DXQuaternionNormalize(&qRotation, &qRotation);

		D3DXQuaternionMultiply(&m_qRotation, &m_qRotation, &qRotation);
		D3DXQuaternionNormalize(&m_qRotation, &m_qRotation);
	}
}

void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void Camera::Render()
{
	D3DXMATRIX mTranslation;
	D3DXMATRIX mRotation;
	
	D3DXVECTOR3 vPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXQUATERNION qRotation;
	
	//D3DXQuaternionIdentity(&qRotation);
	D3DXMatrixIdentity( &m_viewMatrix );
	D3DXMatrixIdentity( &mTranslation );
	
	if (!m_isAutoDriving)
	{
		vPosition = m_position;
		qRotation = m_qRotation;
	}
	else
	{		
		CameraJourneyWithInterpolation(vPosition, qRotation);
	}
	
	D3DXMatrixRotationQuaternion( &m_viewMatrix, &qRotation );
	D3DXMatrixTranslation( &mTranslation, -vPosition.x, -vPosition.y, -vPosition.z );
	D3DXMatrixMultiply( &m_viewMatrix, &mTranslation, &m_viewMatrix);
	
	return;
}

void Camera::CameraJourneyWithInterpolation( D3DXVECTOR3 &vPosition, D3DXQUATERNION &qRotation ) 
{
	int pointsCount = m_savedPoints.size();

/*
	//in order to normalize speed variation between longer paths and shorter paths,
	//we determine the distance between the points and adjust the progress inverse to the distance,
	//which gives a little increase of progress on long ways, and a shorter increase on short ways

	float totalDistance = 0;

	for (int i = 0; i < pointsCount-1; i++)
		totalDistance += D3DXVec3Length(&(m_savedPoints[i].position - m_savedPoints[i+1].position));

	totalDistance += D3DXVec3Length(&(m_savedPoints[pointsCount-1].position - m_savedPoints[0].position));

	float distance = D3DXVec3Length(&(m_savedPoints[(m_pointsCounter+1)%pointsCount].position - m_savedPoints[(m_pointsCounter+2)%pointsCount].position));
*/
	m_progress += SPEED * 0.01f; // SPEED/distance;

	//if full progress reached, set it to 1.0
	if(m_progress > 1.0f)
		m_progress = 1.0f;

	//Perform a Catmull-Rom interpolation, using the specified 3D vectors position
	D3DXVec3CatmullRom(
		&vPosition, 
		&m_savedPoints[m_pointsCounter].position,	// Beginning control point
		&m_savedPoints[(m_pointsCounter+1) % pointsCount].position,	// Beginning position
		&m_savedPoints[(m_pointsCounter+2) % pointsCount].position,	// Ending position
		&m_savedPoints[(m_pointsCounter+3) % pointsCount].position,	// Ending control point
		m_progress);									// Progress according to distance between beginning and ending

	//Interpolate between two quaternions, using spherical linear interpolation.
	D3DXQuaternionSlerp(
		&qRotation, 
		&m_savedPoints[(m_pointsCounter+1) % pointsCount].rotation,	// Beginning rotation
		&m_savedPoints[(m_pointsCounter+2) % pointsCount].rotation, //Ending rotation
		m_progress);

	//if we 'drove' the full way on the section, go on with next section
	if(m_progress >= 1.0f)
	{
		m_progress = 0.0f;
		++m_pointsCounter;

		if(m_pointsCounter == pointsCount)
		{
			m_pointsCounter = 0;
		}
	}
}
void Camera::SavePoint()
{
	point savedPoint;	
	savedPoint.rotation = m_qRotation;
	savedPoint.position = m_position;
	m_savedPoints.push_back(savedPoint);
}


void Camera::RunPoints()
{
	if(m_savedPoints.size() >= 4)
	{
		m_isAutoDriving = true;
	}
}

void Camera::SetSyncTime(float syncTime)
{
	m_updateTime = syncTime;
	return;
}