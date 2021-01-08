// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "../DXFramework/Light.h"
#include "LightShader.h"
#include "TextureShader.h"
#include "TerrainMesh.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);

	bool frame();

protected:
	bool render();

	void RenderScene();
	void RenderToTexture();
	void gui();

	void DownSample();
	void ApplyHBlur();
	void ApplyVBlur();
	void UpSample();

	bool blur;

private:
	TextureShader* textureShader;
	LightShader* lightShader;
	HorizontalBlurShader* hBlurShader;
	VerticalBlurShader* vBlurShader;

	ID3D11ShaderResourceView* textureArray[2];
	
	TerrainMesh* mesh;
	OrthoMesh* orthoMesh;

	OrthoMesh* downSampleMesh;
	OrthoMesh* hBlurMesh;
	OrthoMesh* vBlurMesh;
	OrthoMesh* upSampleMesh;
	
	Light* light;

	RenderTexture* renderTexture;
	RenderTexture* downSampled;
	RenderTexture* hBlurred;
	RenderTexture* vBlurred;
	RenderTexture* finalBlurred;
};
#endif