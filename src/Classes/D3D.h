#ifndef _D3D_H_
#define _D3D_H_

#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d10.h>
#include <d3dx10.h>

class D3D
{
public:
	D3D(int sampleCount, int sampleQuality);
	D3D(const D3D&);
	~D3D();


	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	//FIXME: dirty hack for wireframe mode
	ID3D10RasterizerState* m_rasterState;
	ID3D10RasterizerState* m_rasterStateWire;

	ID3D10Device* GetDevice();
	
	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void SetBackBufferRenderTarget();
	void ResetViewport();

	UINT GetMultisampleQualityLevels(UINT numSamples,DXGI_FORMAT format);

	void __cdecl odprintf(const char *format, ...);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D10Device* m_device;
	ID3D10RenderTargetView* m_renderTargetView;
	ID3D10Texture2D* m_depthStencilBuffer;
	ID3D10DepthStencilState* m_depthStencilState;
	ID3D10DepthStencilView* m_depthStencilView;


	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;
	ID3D10DepthStencilState* m_depthDisabledStencilState;
	D3D10_VIEWPORT m_viewport;

	DXGI_ADAPTER_DESC m_adapterDesc;
	DXGI_SWAP_CHAIN_DESC m_swapChainDesc;
	D3D10_TEXTURE2D_DESC m_depthBufferDesc;
	D3D10_DEPTH_STENCIL_DESC m_depthStencilDesc;
	D3D10_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
	D3D10_RASTERIZER_DESC m_rasterDesc, m_rasterDesc2;
	D3D10_DEPTH_STENCIL_DESC m_depthDisabledStencilDesc;

	UINT m_sampleCount;
	UINT m_sampleQuality;
};

#endif