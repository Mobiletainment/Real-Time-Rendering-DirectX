////////////////////////////////////////////////////////////////////////////////
// Filename: TextureArray.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureArray.h"


TextureArray::TextureArray()
{
	m_textures[0] = 0;
	m_textures[1] = 0;
}


TextureArray::TextureArray(const TextureArray& other)
{
}


TextureArray::~TextureArray()
{
}


bool TextureArray::Initialize(ID3D10Device* device, WCHAR* filename1, WCHAR* filename2)
{
	HRESULT result;


	// Load the first texture in.
	result = D3DX10CreateShaderResourceViewFromFile(device, filename1, NULL, NULL, &m_textures[0], NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Load the second texture in.
	result = D3DX10CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_textures[1], NULL);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void TextureArray::Shutdown()
{
	// Release the texture resources.
	if(m_textures[0])
	{
		m_textures[0]->Release();
		m_textures[0] = 0;
	}

	if(m_textures[1])
	{
		m_textures[1]->Release();
		m_textures[1] = 0;
	}

	return;
}


ID3D10ShaderResourceView** TextureArray::GetTextureArray()
{
	return m_textures;
}