// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "ColourShader.h"
#include "LightShader.h"
#include "TextureShader.h"
#include "ManipulationShader.h"
#include "TessellationShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "TessQuadMesh.h"

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

	bool depth;

	bool blur;

	float rot;
	float swing;
	bool swingUp;

private:
	LightShader* lightShader;
	TextureShader* textureShader;
	ManipulationShader* manipShader;
	TessellationShader* tessShader;
	HorizontalBlurShader* hBlurShader;
	VerticalBlurShader* vBlurShader;

	PlaneMesh* heightMesh;
	SphereMesh* sphere;


	OrthoMesh* orthoMesh;

	OrthoMesh* downSampleMesh;
	OrthoMesh* hBlurMesh;
	OrthoMesh* vBlurMesh;
	OrthoMesh* upSampleMesh;

	Light*  lights[2];

	RenderTexture* renderTexture;
	RenderTexture* downSampled;
	RenderTexture* hBlurred;
	RenderTexture* vBlurred;
	RenderTexture* finalBlurred;
};

#endif