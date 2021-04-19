#include "DirectXRenderer.h"
#include "Portal.h"

DirectXRenderer* DirectXRenderer::pInstance = 0;
Destroyer<DirectXRenderer> DirectXRenderer::destroyer;

LRESULT CALLBACK DirectXRenderer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_DESTROY:
			return 0;
		case WM_KEYDOWN:
			return 0;
		case WM_INPUT:
		{
			getInstance()->getInput()->Update(lParam);
			return 0;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);	
	}
}

//Constructor / Destructor
DirectXRenderer::DirectXRenderer(void)
{
	LogManager::getInstance()->trace("Initializing DirectX...");

	//set everything to NULL
	pd3dDev = NULL;
	lpd3d = NULL;
	pd3dGAMEVDecl = NULL;
	pd3dUIVDecl = NULL;
	pRenderTexture = NULL;
	pRenderSurface = NULL;
	pBackBuffer = NULL;
	pRenderVB = NULL;
	pPostProcess = NULL;
	pLighting = NULL;
	activeScene = NULL;
	bSS = false;
	SSNum = 0;

	//D3DDEVTYPE_HAL should work, if not, use REF
	D3DDEVTYPE d3dDT = D3DDEVTYPE_HAL;

	//set up the window class
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "GameWindow";
	RegisterClass(&wc);

	wWidth = 640;
	wHeight = 480;

	RECT currentRes;
	GetWindowRect(GetDesktopWindow(), &currentRes);
	wWidth = currentRes.right;
	wHeight = currentRes.bottom;

	//wWidth = 1920;
	//wHeight = 1080;
	//wWidth = 1280;
	//wHeight = 800;

	//create the window
	hWnd = CreateWindow("GameWindow", "Game", WS_POPUP | WS_SYSMENU | WS_VISIBLE,
		0, 0, wWidth, wHeight, NULL, NULL, NULL, NULL);

	//create the direct3d thing
	if(NULL == (lpd3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		LogManager::getInstance()->trace("[ERROR] Unable to initialize Direct3D9.");
		MessageBox(NULL, "Unable to initialize Direct3D9", "ERROR!", MB_OK | MB_ICONERROR);
		PostQuitMessage(1);
	}
	else
	{
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));

		// get the current display mode
		D3DDISPLAYMODE sMode;
		lpd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &sMode);

		d3dpp.Windowed = true;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferHeight = wHeight;
		d3dpp.BackBufferWidth = wWidth;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.EnableAutoDepthStencil = true;

		if(SUCCEEDED(lpd3d->CheckDepthStencilMatch(0,d3dDT,D3DFMT_A8R8G8B8,D3DFMT_A8R8G8B8, D3DFMT_D32)))
		{
			d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
		}
		else d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

		lpd3d->GetDeviceCaps(0, D3DDEVTYPE_HAL, &d3dCaps);
		DWORD creationFlags = D3DCREATE_MULTITHREADED;
		if(d3dCaps.VertexShaderVersion >= D3DVS_VERSION(2,0))
			creationFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			creationFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		if(FAILED(lpd3d->CreateDevice(0, d3dDT, hWnd, creationFlags, &d3dpp, &pd3dDev)))
		{
			LogManager::getInstance()->trace("[ERROR] Unable to create the Direct 3D Device.");
			MessageBox(NULL, "Unable to create the Direct 3D Device", "ERROR!", MB_OK | MB_ICONERROR);
			PostQuitMessage(1);
		}

		lpd3d->Release();
		lpd3d = NULL;

		D3DVERTEXELEMENT9* vdecl = VERTEX3D::GetDeclarationElements();
		
		if(FAILED(pd3dDev->CreateVertexDeclaration(vdecl, &pd3dGAMEVDecl)))
		{
			LogManager::getInstance()->trace("[ERROR] Failed to create the 3D vertex decleration");
			PostQuitMessage(1);
		}

		D3DVERTEXELEMENT9* vdecl2 = VERTEX2D::GetDeclarationElements();
		
		if(FAILED(pd3dDev->CreateVertexDeclaration(vdecl2, &pd3dUIVDecl)))
		{
			LogManager::getInstance()->trace("[ERROR] Failed to create the UI vertex decleration");
			PostQuitMessage(1);
		}
		
		pd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		pd3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);
		pd3dDev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

		VERTEX2D temp[] = {{-1.f, -1.f, 0.f, 1.0f, 0.0f, 0.0f},
										{(float)wWidth, -1.f, 0.f, 1.0f, 1.0f, 0.0f},
										{-1.f, (float)wHeight, 0.f, 1.0f, 0.0f, 1.0f},
										{(float)wWidth, (float)wHeight, 0.f, 1.0f, 1.0f, 1.0f}};

		pd3dDev->CreateVertexBuffer(	
			sizeof(temp),
			D3DUSAGE_WRITEONLY,
			NULL,
			D3DPOOL_MANAGED,
			&pRenderVB,
			NULL);
		VOID* pVoid;
		pRenderVB->Lock(0, 0, (void**)&pVoid, 0);
		memcpy(pVoid, temp, sizeof(temp));
		pRenderVB->Unlock();
		pd3dDev->CreateTexture(wWidth, wHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pRenderTexture, NULL);
		pRenderTexture->GetSurfaceLevel(0, &pRenderSurface);

		char *file = "Resources/Shaders/Default.rfx";
		ID3DXBuffer* buffer;
		if(FAILED(D3DXCreateEffectFromFile(pd3dDev, file, /*macros*/NULL, NULL, 0, NULL, &pPostProcess, &buffer)))
		{
			if(buffer)
			{
				LogManager::getInstance()->trace("[ERROR] HLSL: %s", buffer->GetBufferPointer());
				buffer->Release();
			}
		}

		char *file2 = "Resources/Shaders/Lighting.rfx";
		if(FAILED(D3DXCreateEffectFromFile(pd3dDev, file2, /*macros*/NULL, NULL, 0, NULL, &pLighting, &buffer)))
		{
			if(buffer->GetBufferSize())
			{
				LogManager::getInstance()->trace("[ERROR] HLSL: %s", buffer->GetBufferPointer());
				buffer->Release();
			}
		}

		//Set DebugCube stuff
		{
			VERTEX3D cubeVerts[] = {
				//FRONT (1-2-3-4)
				{-0.5f, 0.5f, -0.5f,	0.f, 0.f, -1.f, 0.f, 0.f,		0.f,-1.f,0.f, -1.f,0.f,0.f},
				{0.5f, 0.5f, -0.5f,		0.f, 0.f, -1.f, 1.f, 0.f,		0.f,-1.f,0.f, -1.f,0.f,0.f},
				{-0.5f, -0.5f, -0.5f,	0.f, 0.f, -1.f, 0.f, 1.f,		0.f,-1.f,0.f, -1.f,0.f,0.f},
				{0.5f, -0.5f, -0.5f,	0.f, 0.f, -1.f, 1.f, 1.f,		0.f,-1.f,0.f, -1.f,0.f,0.f},
				//RIGHT (2-6-4-8)
				{0.5f, 0.5f, -0.5f,		1.f, 0.f, 0.f, 0.f, 0.f,		0.f,1.f,0.f, 0.f,0.f,1.f},
				{0.5f, 0.5f, 0.5f,		1.f, 0.f, 0.f, 1.f, 0.f,		0.f,1.f,0.f, 0.f,0.f,1.f},
				{0.5f, -0.5f, -0.5f,	1.f, 0.f, 0.f, 0.f, 1.f,		0.f,1.f,0.f, 0.f,0.f,1.f},
				{0.5f, -0.5f, 0.5f,		1.f, 0.f, 0.f, 1.f, 1.f,		0.f,1.f,0.f, 0.f,0.f,1.f},
				//TOP (5-6-1-2)
				{-0.5f, 0.5f, 0.5f,		0.f, 1.f, 0.f, 0.f, 0.f,		1.f,0.f,0.f, 0.f,0.f,1.f},
				{0.5f, 0.5f, 0.5f,		0.f, 1.f, 0.f, 1.f, 0.f,		1.f,0.f,0.f, 0.f,0.f,1.f},
				{-0.5f, 0.5f, -0.5f,	0.f, 1.f, 0.f, 0.f, 1.f,		1.f,0.f,0.f, 0.f,0.f,1.f},
				{0.5f, 0.5f, -0.5f,		0.f, 1.f, 0.f, 1.f, 1.f,		1.f,0.f,0.f, 0.f,0.f,1.f},
				//BACK (6-5-8-7)
				{0.5f, 0.5f, 0.5f,		0.f, 0.f, 1.f, 0.f, 0.f,		1.f,0.f,0.f, 0.f,1.f,0.f},
				{-0.5f, 0.5f, 0.5f,		0.f, 0.f, 1.f, 1.f, 0.f,		1.f,0.f,0.f, 0.f,1.f,0.f},
				{0.5f, -0.5f, 0.5f,		0.f, 0.f, 1.f, 0.f, 1.f,		1.f,0.f,0.f, 0.f,1.f,0.f},
				{-0.5f, -0.5f, 0.5f,	0.f, 0.f, 1.f, 1.f, 1.f,		1.f,0.f,0.f, 0.f,1.f,0.f},
				//LEFT (5-1-7-3)
				{-0.5f, 0.5f, 0.5f,		-1.f, 0.f, 0.f, 0.f, 0.f,		0.f,-1.f,0.f, 0.f,0.f,-1.f},
				{-0.5f, 0.5f, -0.5f,	-1.f, 0.f, 0.f, 1.f, 0.f,		0.f,-1.f,0.f, 0.f,0.f,-1.f},
				{-0.5f, -0.5f, 0.5f,	-1.f, 0.f, 0.f, 0.f, 1.f,		0.f,-1.f,0.f, 0.f,0.f,-1.f},
				{-0.5f, -0.5f, -0.5f,	-1.f, 0.f, 0.f, 1.f, 1.f,		0.f,-1.f,0.f, 0.f,0.f,-1.f},
				//BOTTOM (3-4-7-8)
				{-0.5f, -0.5f, -0.5f,	0.f, -1.f, 0.f, 0.f, 0.f,		-1.f,0.f,0.f, 0.f,0.f,-1.f},
				{0.5f, -0.5f, -0.5f,	0.f, -1.f, 0.f, 1.f, 0.f,		-1.f,0.f,0.f, 0.f,0.f,-1.f},
				{-0.5f, -0.5f, 0.5f,	0.f, -1.f, 0.f, 0.f, 1.f,		-1.f,0.f,0.f, 0.f,0.f,-1.f},
				{0.5f, -0.5f, 0.5f,		0.f, -1.f, 0.f, 1.f, 1.f,		-1.f,0.f,0.f, 0.f,0.f,-1.f}
			};

			if(FAILED(pd3dDev->CreateVertexBuffer(sizeof(cubeVerts), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &(DebugCube::pVB), NULL)))
				LogManager::getInstance()->trace("[ERROR] Could not create Vertex Buffer for the Cube");

			VERTEX3D* pData;
			(DebugCube::pVB)->Lock(NULL, NULL, (void**)&pData, NULL);
			memcpy(pData, cubeVerts, sizeof(cubeVerts));
			(DebugCube::pVB)->Unlock();

			if(FAILED(D3DXCreateTextureFromFile(pd3dDev, "Resources/Textures/Default.rtf", &(DebugCube::pDefault))))
				LogManager::getInstance()->trace("[ERROR] Could not create Default Texture");
		}

		{
			VERTEX3D plane[] = {
				{-0.5f, 0.0f, 0.5f,		0.f, 1.f, 0.f, 0.f, 0.f,		1.f,0.f,0.f, 0.f,0.f,1.f},
				{0.5f, 0.0f, 0.5f,		0.f, 1.f, 0.f, 1.f, 0.f,		1.f,0.f,0.f, 0.f,0.f,1.f},
				{-0.5f, 0.0f, -0.5f,	0.f, 1.f, 0.f, 0.f, 1.f,		1.f,0.f,0.f, 0.f,0.f,1.f},
				{0.5f, 0.0f, -0.5f,		0.f, 1.f, 0.f, 1.f, 1.f,		1.f,0.f,0.f, 0.f,0.f,1.f}
			};

			if(FAILED(pd3dDev->CreateVertexBuffer(sizeof(plane), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &(Portal::pVB), NULL)))
				LogManager::getInstance()->trace("[ERROR] Could not create Vertex Buffer for the Portal");

			VERTEX3D* pData;
			(Portal::pVB)->Lock(NULL, NULL, (void**)&pData, NULL);
			memcpy(pData, plane, sizeof(plane));
			(Portal::pVB)->Unlock();

			if(FAILED(D3DXCreateTextureFromFile(pd3dDev, "Resources/Textures/Orange.rtf", &(Portal::pTexture[0]))))
				LogManager::getInstance()->trace("[ERROR] Could not create Portal0 Texture");
			if(FAILED(D3DXCreateTextureFromFile(pd3dDev, "Resources/Textures/Blue.rtf", &(Portal::pTexture[1]))))
				LogManager::getInstance()->trace("[ERROR] Could not create Portal1 Texture");

			SoundEngine::getInstance()->createSoundEffect("portal_enter1.wav", &Portal::sPortEnter[0]);
			SoundEngine::getInstance()->createSoundEffect("portal_enter2.wav", &Portal::sPortEnter[1]);
		}

		LogManager::getInstance()->trace("DirectX successfully initialized.");
	}

	pInput = new Input(Input::INPUT_DI);

	initHavok();
}
DirectXRenderer::~DirectXRenderer(void){}

//Rendering / Main Loops
void DirectXRenderer::beginRender(void)
{
	//Gets the render target and sets it to pBackBuffer
	pd3dDev->GetRenderTarget(0, &pBackBuffer);

	//Shadows
	

	//sets the render target to our own surface
	pd3dDev->SetRenderTarget(0, pRenderSurface);
	//usual clears and begin the scene
	pd3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	pd3dDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	pd3dDev->BeginScene();
};
void DirectXRenderer::endRender(void)
{
	//end the scene
	pd3dDev->EndScene();

	//Do RTT
	//send the texture to the Shader
	pPostProcess->SetTexture("Screen", pRenderTexture);
	//set the technique to be performed
	pPostProcess->SetTechnique(pPostProcess->GetTechniqueByName("Blur"));
	//set the render target back to the backbuffer
	pd3dDev->SetRenderTarget(0, pBackBuffer);
	//clear and begin scene
	pd3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(100, 0, 0), 1.0f, 0);
	pd3dDev->BeginScene();
	//set the vertex decleration to UI because this will be in 2D
	setUIVertDecl();
	//set texture and Vertex Buffer
	pd3dDev->SetTexture(0, pRenderTexture);
	pd3dDev->SetStreamSource(0, pRenderVB, 0, sizeof(VERTEX2D));
	UINT cPasses;
	//run the shader code
	pPostProcess->Begin(&cPasses, 0);
	for(UINT i = 0; i < cPasses; i++)
	{
		pPostProcess->BeginPass(i);
		pd3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
		pPostProcess->EndPass();
	}
	pPostProcess->End();
	pd3dDev->SetTexture(0, NULL);
	pd3dDev->EndScene();

	//present, set the mouse state back to 0, and set mouse back to center
	pd3dDev->Present(NULL, NULL, NULL, NULL);
	SetCursorPos(wWidth/2,wHeight/2);
	if(pInput != NULL)
		pInput->Reset();

	//capture screen if screenshot function called.
	if(bSS) internalScreenCap();
}
WPARAM DirectXRenderer::MainLoop(void)
{
	bQuit = false;
	MSG msg;

	QueryPerformanceCounter(&nStartTime);
	QueryPerformanceFrequency(&nFreq);

	ShowCursor(false);

	while(!bQuit)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT) bQuit = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		QueryPerformanceCounter(&nStopTime);
		double dTime = ((nStopTime.QuadPart - nStartTime.QuadPart) * 1000.0 / nFreq.QuadPart);
		SoundEngine::getInstance()->update();
		getInstance()->getInput()->Update();
#ifdef _DEBUG
		visualDebugger->step();
#endif
		if(activeScene != NULL && activeScene->isLoading != true)
		{
			if(!activeScene->isLoading) activeScene->Update(dTime);
			beginRender();
			if(!activeScene->isLoading) activeScene->Render();
			endRender();
		}
		nStartTime = nStopTime;
	}
	//Kill Havok
#ifdef _DEBUG
	visualDebugger->shutdown();
	visualDebugger->removeReference();
#endif

	//Kill D3D
	SAFE_DELETE(activeScene);
	SAFE_RELEASE(pBackBuffer);
	SAFE_RELEASE(pLighting);
	SAFE_RELEASE(pPostProcess);
	SAFE_RELEASE(pRenderSurface);
	SAFE_RELEASE(pRenderTexture);
	SAFE_RELEASE(pRenderVB);
	SAFE_RELEASE(pd3dUIVDecl);
	SAFE_RELEASE(pd3dGAMEVDecl);
	SAFE_RELEASE(pd3dDev);
	SAFE_RELEASE(lpd3d);

	return msg.wParam;
}

//Transformations
void DirectXRenderer::translate(Vector v)
{
	D3DXMATRIX temp, tempWorld;
	D3DXMatrixTranslation(&temp, v.GetX(), v.GetY(), v.GetZ());
	pd3dDev->GetTransform(D3DTS_WORLD, &tempWorld);
	pd3dDev->SetTransform(D3DTS_WORLD, &(tempWorld * temp));
}
void DirectXRenderer::scale(Vector v)
{
	D3DXMATRIX temp, tempWorld;
	D3DXMatrixScaling(&temp, v.GetX(), v.GetY(), v.GetZ());
	pd3dDev->GetTransform(D3DTS_WORLD, &tempWorld);
	pd3dDev->SetTransform(D3DTS_WORLD, &(tempWorld * temp));
}
void DirectXRenderer::scale(float f)
{
	D3DXMATRIX temp, tempWorld;
	D3DXMatrixScaling(&temp, f, f, f);
	pd3dDev->GetTransform(D3DTS_WORLD, &tempWorld);
	pd3dDev->SetTransform(D3DTS_WORLD, &(tempWorld * temp));
}
void DirectXRenderer::rotateYPR(Vector v)
{
	D3DXMATRIX temp, tempWorld;
	D3DXMatrixRotationYawPitchRoll(&temp, v.GetY(), v.GetX(), v.GetZ());
	pd3dDev->GetTransform(D3DTS_WORLD, &tempWorld);
	pd3dDev->SetTransform(D3DTS_WORLD, &(tempWorld * temp));
}
void DirectXRenderer::rotateAxis(Vector v, float f)
{
	D3DXMATRIX temp, tempWorld;
	D3DXMatrixRotationAxis(&temp, &v.dxGetVector(), f);
	pd3dDev->GetTransform(D3DTS_WORLD, &tempWorld);
	pd3dDev->SetTransform(D3DTS_WORLD, &(tempWorld * temp));
}
void DirectXRenderer::pushMatrix(void) 
{ 
	D3DXMATRIX temp;
	pd3dDev->GetTransform(D3DTS_WORLD, &temp);
	vMatrixStack.push_back(temp);
}
void DirectXRenderer::popMatrix(void)
{
	if(vMatrixStack.empty()) return;
	pd3dDev->SetTransform(D3DTS_WORLD, &vMatrixStack.back());
	vMatrixStack.pop_back();
}
void DirectXRenderer::multiplyMatrixWithWorld(D3DXMATRIX *mat)
{
	D3DXMATRIX tempOut, tempWorld;
	pd3dDev->GetTransform(D3DTS_WORLD, &tempWorld);
	D3DXMatrixMultiply(&tempOut, &tempWorld, mat);
	pd3dDev->SetTransform(D3DTS_WORLD, &tempOut);
}

//Vertex Declerations
void DirectXRenderer::setUIVertDecl(void){pd3dDev->SetVertexDeclaration(pd3dUIVDecl);}
void DirectXRenderer::setGameVertDecl(void){pd3dDev->SetVertexDeclaration(pd3dGAMEVDecl);}

//Input Operations
Input* DirectXRenderer::getInput(void)
{
	return pInput;
}

//Havok
hkpWorld* DirectXRenderer::getHKWorld(void){return world;}
void DirectXRenderer::initHavok(void)
{
	LogManager::getInstance()->trace("Initializing HavokBase...");

	memoryRouter = hkMemoryInitUtil::initDefault(&baseMalloc, hkMemorySystem::FrameInfo(6*MEGABYTE));
	hkBaseSystem::init(memoryRouter, havokErrorReport);

	initHavokPhysics();
	//initHavokAnimation();

#ifdef _DEBUG
	//For the Visual Debugger.
	physicsContext = new hkpPhysicsContext();
	hkpPhysicsContext::registerAllPhysicsProcesses();
	physicsContext->addWorld(world);
	hkArray<hkProcessContext*> contexts;
	contexts.pushBack(physicsContext);

	visualDebugger = new hkVisualDebugger(contexts);
	visualDebugger->serve();
	//end Visual Debugger stuff.
	LogManager::getInstance()->trace("Havok Visual Debugger Active.");
#endif

	LogManager::getInstance()->trace("HavokBase Successfully Initialized.");
}
void DirectXRenderer::initHavokPhysics(void)
{
	LogManager::getInstance()->trace("Initializing HavokPhysics...");

	hkpWorldCinfo info;
	info.setBroadPhaseWorldSize(1000.0f);
	info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_HARD);
	info.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_FIX_ENTITY;
	world = new hkpWorld(info);
	world->lock();
	hkpAgentRegisterUtil::registerAllAgents(world->getCollisionDispatcher());

	world->unlock();
		
	LogManager::getInstance()->trace("HavokPhysics Successfully Initialized.");
}
void DirectXRenderer::killHavok(void)
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

//Effects
void DirectXRenderer::updateLighting(void)
{
	D3DXMATRIX world, view, proj, worldinvtrans, temp;
	pd3dDev->GetTransform(D3DTS_WORLD, &world);
	pd3dDev->GetTransform(D3DTS_VIEW, &view);
	pd3dDev->GetTransform(D3DTS_PROJECTION, &proj);
	pLighting->SetMatrix("World", &world);
	pLighting->SetMatrix("View", &view);
	pLighting->SetMatrix("Projection", &proj);
	D3DXMatrixInverse(&temp, NULL, &world);
	D3DXMatrixTranspose(&worldinvtrans, &temp);
	pLighting->SetMatrix("WorldInverseTranspose", &worldinvtrans);
}
ID3DXEffect* DirectXRenderer::getLightingEffect(void){return pLighting;}

//Returns and other simple operations
IDirect3DDevice9* DirectXRenderer::getDevice(void){return pd3dDev;}
LPDIRECT3D9 DirectXRenderer::getD3D(void){return lpd3d;}
D3DCAPS9 DirectXRenderer::getCaps(void){return d3dCaps;}
UINT16 DirectXRenderer::getWidth(void){return wWidth;}
UINT16 DirectXRenderer::getHeight(void){return wHeight;}
float DirectXRenderer::getAspectRatio(void){return ((float)wWidth/(float)wHeight);}
void DirectXRenderer::setActiveScene(Scene* scene)
{
	if(activeScene != NULL)
	{
		delete activeScene;
		activeScene = NULL;
	}
	activeScene=scene;
}
Scene* DirectXRenderer::getActiveScene(void){return activeScene;}
UINT DirectXRenderer::run(void){return MainLoop();}
void DirectXRenderer::run(UINT *retval){*retval = MainLoop();}
void DirectXRenderer::quitProgram(int code){bQuit=true;}
void DirectXRenderer::screenshot(void){bSS = true;}
void DirectXRenderer::internalScreenCap(void)
{
	char temp[64];
	//format time so that it will be accepted as a file name
	sprintf_s(temp, "%s%d.png", "Screenshot", SSNum);
	D3DXSaveTextureToFile(temp, D3DXIFF_PNG, pRenderTexture, NULL);
	SSNum++;
	bSS = false;
}