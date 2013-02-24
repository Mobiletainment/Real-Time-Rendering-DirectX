#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "SpotLight.h"
#include <vector>
//new in V2: headers for the render2texture, depth shader, and shadow shader classes. 
#include "DepthShader.h"
#include "Render2Texture.h"
#include "ShadowShader.h"
#include "ShadowAndBumpShader.h"
#include "IShadowShader.h"
using namespace std;

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

//the shadow map size can easily be tweaked. 
const int SHADOWMAP_WIDTH = 1280*2.5; //displaying resolution = 1280x800
const int SHADOWMAP_HEIGHT = 800*2.5; //currently 2,5 of displayed resolution
const int ROWS = 5;

class Graphics
{
public:
	enum RenderMode
	{
		RenderModeShadowMap,
		RenderModeBumpAndShadowMap
	};



	void ActivateBumpMapRenderMode();
	void ActivateShadowMapRenderMode();
	void ToggleWireFrameMode(bool switchOn);

	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	RenderMode CurrentRenderMode;

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, int currentSampleCount, int currentSampleQuality);

	bool InitializeD3D(int screenWidth, int screenHeight, HWND hwnd, int currentSampleCount, int currentSampleQuality);
	int GetMultiSampleQualityLevel(int sampleCount);
	void Shutdown();
	void ShutdownD3D();

	bool Update(float syncTime);
	void UpdateCameraOrientation(float mouseDeltaX, float mouseDeltaY, float roll);
	void UpdateLightDirection(bool forward);
	void UpdateLightPosition(float forward, float sideways, float upwards);
	void UpdateCameraMovement(float moveForward, float moveSideways);
	void SavePoint();
	void RunPoints();
	bool Render();

private:
	D3D* m_D3D;
	Camera* m_Camera;
	vector<Model*> *m_Models;
	Model *m_Cube;
	bool m_isWireFrameEnabled;
	IShadowShader* m_CurrentShader;
	ShadowShader* m_ShadowShader;
	ShadowAndBumpShader* m_ShadowBumpShader;
	SpotLight* m_Light;



	//new in v2: adjustments for shadow mapping
	bool RenderSceneToTexture();
	DepthShader* m_DepthShader;
	Render2Texture* m_Render2Texture;
	Model* m_SunSphere;
};