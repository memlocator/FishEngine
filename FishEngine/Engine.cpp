#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
	delete DxHandler::firstPassPixel;
	delete DxHandler::secondPassPixel;

	delete DxHandler::firstPassVertex;
	delete DxHandler::secondPassVertex;
}

void Engine::initialSetup()
{
	DxHandler::WIDTH = WIDTH;
	DxHandler::HEIGHT = HEIGHT;

	this->createWindow();
	createDirectX();
	DxHandler::firstPassPixel->compileShader(L"./FirstPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::firstPassVertex->compileShader(L"./FirstPassVertex.hlsl", DxHandler::devicePtr);

	DxHandler::secondPassPixel->compileShader(L"./SecondPassPixel.hlsl", DxHandler::devicePtr);
	DxHandler::secondPassVertex->compileShader(L"./SecondPassVertex.hlsl", DxHandler::devicePtr);

	deferredBufferHandler.init(WIDTH, HEIGHT);

	directXHandler->setupInputLayout();
	directXHandler->setupDepthBuffer();

	DxHandler::contextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DxHandler::contextPtr->IASetInputLayout((ID3D11InputLayout*)DxHandler::input_layout_ptr);

	PS_CONSTANT_LIGHT_BUFFER ps_buff;
	VS_CONSTANT_MATRIX_BUFFER vs_buff;
	directXHandler->createPSConstBuffer(ps_buff);
	directXHandler->createVSConstBuffer(vs_buff);
}

void Engine::fixedUpdate(double deltaTime) //time in seconds since last frame
{
	//game logic here thanks
}

void Engine::createWindow()
{
	this->window = RenderWindow();
	this->primaryWindow = this->window.createWindow(WIDTH, HEIGHT, "Fish Engine", "Title");
}

void Engine::createDirectX()
{
	this->directXHandler = new DxHandler(primaryWindow);
}

void Engine::createInputHandler()
{
	this->inputHandler = InputHandler(primaryWindow);
}

void Engine::engineLoop()
{
	RECT viewportRect;
	GetClientRect(primaryWindow, &viewportRect);
	D3D11_VIEWPORT port = {
		0.f,
		0.f,
		(float)(viewportRect.right - viewportRect.left),
		(float)(viewportRect.bottom - viewportRect.top),0.f,1.f
	};
	port.MinDepth = 0.0f; //Closest possible to screen Z depth
	port.MaxDepth = 1.0f; //Furthest possible
	float background_color[4] = { 0.5f, 0.f, 0.f, 1.f };
	directXHandler->contextPtr->RSSetViewports(1, &port);
	DxHandler::contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);
	//--------------------------------------------------------------------------//
	Mesh debugObject;
	debugObject.readMeshFromFile("./Models/actualCube.obj");
	debugObject.setTranslation(DirectX::XMFLOAT3(0, 0, 3));

	Camera debugCam = Camera(WIDTH, HEIGHT);

	MSG msg;
	while (true)
	{
		//Clear RTVs again
		directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Position].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(deferredBufferHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);

		ID3D11RenderTargetView* arr[3] =
		{
			deferredBufferHandler.buffers[GBufferType::Position].renderTargetView,
			deferredBufferHandler.buffers[GBufferType::DiffuseColor].renderTargetView,
			deferredBufferHandler.buffers[GBufferType::Normal].renderTargetView,
		};
		directXHandler->contextPtr->OMSetRenderTargets(3, arr, DxHandler::depthStencil); //DEPTH
		DxHandler::firstPassPixel->useThis(DxHandler::contextPtr);
		DxHandler::firstPassVertex->useThis(DxHandler::contextPtr);

		directXHandler->draw(&debugObject, debugCam);

		currentTime = std::chrono::high_resolution_clock::now(); //Reset time
		directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);


		//upp upp och iv����g
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		if (msg.message == WM_DESTROY)
		{
			PostQuitMessage(0);
		}

		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now(); //Set new time
		std::chrono::duration<double> frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime); //Get deltaTime for frame
		fixedUpdate(frameTime.count());
		DxHandler::swapChainPtr->Present(1, 0); 
	}


}
