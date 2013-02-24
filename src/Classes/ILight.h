#include <d3dx10math.h>

class ILight
{
public:
	virtual D3DXVECTOR4 GetAmbientColor() = 0;
	virtual D3DXVECTOR4 GetDiffuseColor() = 0;
	virtual D3DXVECTOR3 GetPosition() = 0;
};

