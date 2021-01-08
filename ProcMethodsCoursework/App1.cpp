// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	//BaseApplication::BaseApplication();
	mesh = nullptr;
	light = nullptr;
	downSampleMesh = nullptr;
	vBlurMesh = nullptr;
	hBlurMesh = nullptr;
	upSampleMesh = nullptr;
	orthoMesh = nullptr;

	textureArray[0] = nullptr;
	textureArray[1] = nullptr;
	downSampled = nullptr;
	hBlurred = nullptr;
	vBlurred = nullptr;
	finalBlurred = nullptr;

	textureShader = nullptr;
	lightShader = nullptr;
	hBlurShader = nullptr;
	vBlurShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{

	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	mesh = new TerrainMesh(renderer->getDevice(), 128, 128);

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	hBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	vBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);

	textureMgr->loadTexture("grass", L"../res/grass.png");
	textureMgr->loadTexture("stone", L"../res/stone.png");

	textureArray[0] = textureMgr->getTexture("grass");
	textureArray[1] = textureMgr->getTexture("stone");

	light = new Light();

	light->setAmbientColour(0.1f, 0.1, 0.1f, 1.0f);
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(1.0f, -0.5f, 0.0f);

	// Create the RenderTexture for the top right view
	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Create the RenderTextures for the blurring
	downSampled = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	hBlurred = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	vBlurred = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	finalBlurred = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Create the orthomeshes for blurring
	downSampleMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2);
	hBlurMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	vBlurMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	upSampleMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth * 2, screenHeight * 2);


	// ortho size and position set based on window size
	// Position default at 0x0 centre window, to offset change values (pixel)
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 64, 10);

	blur = false;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (light)
	{
		delete light;
		light = 0;
	}

	if (downSampleMesh)
	{
		delete downSampleMesh;
		downSampleMesh = 0;
	}

	if (vBlurMesh)
	{
		delete vBlurMesh;
		vBlurMesh = 0;
	}

	if (hBlurMesh)
	{
		delete hBlurMesh;
		hBlurMesh = 0;
	}

	if (upSampleMesh)
	{
		delete upSampleMesh;
		upSampleMesh = 0;
	}

	if (orthoMesh)
	{
		delete orthoMesh;
		orthoMesh = 0;
	}

	if (textureArray)
	{
		delete textureArray;
		textureArray[0] = 0;
		textureArray[1] = 0;
	}

	if (downSampled)
	{
		delete downSampled;
		downSampled = 0;
	}

	if (hBlurred)
	{
		delete hBlurred;
		hBlurred = 0;
	}

	if (vBlurred)
	{
		delete vBlurred;
		vBlurred = 0;
	}

	if (finalBlurred)
	{
		delete finalBlurred;
		finalBlurred = 0;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}

	if (hBlurShader)
	{
		delete hBlurShader;
		hBlurShader = 0;
	}

	if (vBlurShader)
	{
		delete vBlurShader;
		vBlurShader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// Render the scene to the RenderTexture
	RenderToTexture();

	// Render the blurred scene to a texture
	if (blur) {
		DownSample();
		ApplyHBlur();
		ApplyVBlur();
		UpSample();
	}
	// Render the scene to the screen
	RenderScene();

	if (input->isKeyDown('N'))
	{
		mesh->GenerateNoise(renderer->getDevice(), input->isKeyDown('N'));
	}

	if (input->isKeyDown('M'))
	{
		mesh->SmoothTerrain(renderer->getDevice(), input->isKeyDown('M'));
	}

	if (input->isKeyDown('F'))
	{
		mesh->FaultTerrain(renderer->getDevice(), input->isKeyDown('F'));
	}

	return true;
}

void App1::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-64.0f, -10.0f, 32.0f);

	//// Send geometry data (from mesh)
	mesh->Render(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	//texShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("default"));
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureArray, light);
	//// Render object (combination of mesh geometry and shader process
	//texShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	lightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	renderer->setZBuffer(false);
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();
	orthoMesh->sendData(renderer->getDeviceContext());
	if (blur)								// Applies blur
	{
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, finalBlurred->getShaderResourceView());
	}
	else
	{
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTexture->getShaderResourceView());
	}
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Render GUI
	gui();

	//// Present the rendered scene to the screen.
	renderer->endScene();

	

}

void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());

	// Blur Mode Button
	if (ImGui::Button("Blur"))
	{
		blur = !blur;
	}

	// Render UI
	ImGui::Render();
}

void App1::RenderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	// Set the render target to be the render to texture.
	renderTexture->setRenderTarget(renderer->getDeviceContext());
	// Clear the render to texture.
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	// Generate the view matrix based on the camera's position.
	camera->update();
	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-64.0f, -10.0f, 32.0f);

	//// Send geometry data (from mesh)
	mesh->Render(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	//texShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("default"));
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureArray, light);
	//// Render object (combination of mesh geometry and shader process
	//texShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	lightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// Halves the size of the renderTexture
void App1::DownSample()
{
	XMMATRIX worldMatrix, orthoViewMatrix, orthoMatrix;
	// Sets render target to a rendertexture which is half the size of the screen
	downSampled->setRenderTarget(renderer->getDeviceContext());
	downSampled->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	renderer->setZBuffer(false);

	downSampleMesh->sendData(renderer->getDeviceContext());

	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), downSampleMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

// Applies the horizontal blur to the downsampled texture
void App1::ApplyHBlur()
{
	XMMATRIX worldMatrix, orthoViewMatrix, orthoMatrix;

	hBlurred->setRenderTarget(renderer->getDeviceContext());
	hBlurred->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();


	hBlurMesh->sendData(renderer->getDeviceContext());

	hBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downSampled->getShaderResourceView(), downSampled->getTextureWidth());
	hBlurShader->render(renderer->getDeviceContext(), hBlurMesh->getIndexCount());


	renderer->setBackBufferRenderTarget();
}

// Applies the vertical blur to the hblurred texture
void App1::ApplyVBlur()
{
	XMMATRIX worldMatrix, orthoViewMatrix, orthoMatrix;

	vBlurred->setRenderTarget(renderer->getDeviceContext());
	vBlurred->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();


	vBlurMesh->sendData(renderer->getDeviceContext());

	vBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, hBlurred->getShaderResourceView(), hBlurred->getTextureHeight());
	vBlurShader->render(renderer->getDeviceContext(), vBlurMesh->getIndexCount());


	renderer->setBackBufferRenderTarget();
}

// Upsamples the blurred image to the original screen size
void App1::UpSample()
{
	XMMATRIX worldMatrix, orthoViewMatrix, orthoMatrix;

	finalBlurred->setRenderTarget(renderer->getDeviceContext());
	finalBlurred->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	renderer->setZBuffer(false);

	upSampleMesh->sendData(renderer->getDeviceContext());

	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, vBlurred->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), upSampleMesh->getIndexCount());

	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}