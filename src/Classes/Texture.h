#ifndef _Texture_H_
#define _Texture_H_

#include <d3d10.h>
#include <d3dx10.h>

class Texture
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D10Device*, WCHAR*);
	void Shutdown();

	ID3D10ShaderResourceView* GetTexture();

private:
	ID3D10ShaderResourceView* m_texture;
};

#endif