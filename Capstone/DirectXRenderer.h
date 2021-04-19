#ifndef __DXRENDER_H_
#define __DXRENDER_H_

//DirectX
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

//std stuff
#include <vector>
#include <new>
#include <map>

//Assimp
#include "assimp\include\assimp.h"

//Havok Common
#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Fwd/hkcstdio.h>
#ifdef _DEBUG
#include <Common/Visualize/hkVisualDebugger.h>
#endif
// Physics
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>
#include <Physics/Utilities/CharacterControl/FirstPersonCharacter/hkpFirstPersonCharacter.h>
#include <Physics/Utilities/Weapons/hkpGravityGun.h>
#include <Physics/Utilities/Weapons/hkpFirstPersonGun.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBodyListener.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpDefaultCharacterStates.h>
#include <Physics/Internal/Collide/StaticCompound/hkpStaticCompoundShape.h>
#include <Physics/Collide/Shape/Misc/PhantomCallback/hkpPhantomCallbackShape.h>
#include <Physics/Collide/Shape/Misc/Bv/hkpBvShape.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>
#include <Physics/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>

//My Stuff
#include "Input.h"
#include "RenderablePhysics.h"
#include "SoundEngine.h"
#include "Vector.h"
#include "DebugCube.h"
#include "LogManager.h"
#include "Destroyer.h"
#include "Scene.h"

// in order for std::min and std::max to behave properly
#ifdef min 
#undef min
#endif // min

#ifdef max 
#undef max
#endif // max

#define KILOBYTE 1024
#define MEGABYTE 1024*1024
#define SAFE_RELEASE(p) {if(p!=NULL){p->Release();p=NULL;}}
#define SAFE_DELETE(p) {if(p!=NULL){delete p;p=NULL;}}

class Input;

class DirectXRenderer
{
	friend class Destroyer<DirectXRenderer>;
public:

	/********************************************************************************************
	Summary:	Returns the current instance for the renderer. 
				Creates a new one if not already initialized.
	Parameters: None.
	Returns:	DirectXRenderer*.
	********************************************************************************************/
	static inline DirectXRenderer* getInstance(void)
	{
		if(pInstance == NULL)
		{
			pInstance = new DirectXRenderer();
			destroyer.setSingleton(pInstance);
		}
		return pInstance;
	};

	/********************************************************************************************
	Summary:	Returns the DirectX9 Device.
	Parameters: None.
	Returns:	IDirect3DDevice9*.
	********************************************************************************************/
	IDirect3DDevice9* getDevice(void);

	/********************************************************************************************
	Summary:	Returns the Direct3D Variable.
	Parameters: None.
	Returns:	LPDIRECT3D9.
	********************************************************************************************/
	LPDIRECT3D9 getD3D(void);

	/********************************************************************************************
	Summary:	Returns the device capabilities variable.
	Parameters: None.
	Returns:	D3DCAPS9.
	********************************************************************************************/
	D3DCAPS9 getCaps(void);

	/********************************************************************************************
	Summary:	Returns the width of the window.
	Parameters: None.
	Returns:	UINT16.
	********************************************************************************************/
	UINT16 getWidth(void);

	/********************************************************************************************
	Summary:	Returns the height of the window.
	Parameters: None.
	Returns:	UINT16.
	********************************************************************************************/
	UINT16 getHeight(void);

	/********************************************************************************************
	Summary:	Returns the aspect ratio of the window.
	Parameters: None.
	Returns:	float.
	********************************************************************************************/
	float getAspectRatio(void);

	/********************************************************************************************
	Summary:	Sets the active Scene.
	Parameters: [in] scene - Scene to set as active.
	********************************************************************************************/
	void setActiveScene(Scene* scene);

	Scene* getActiveScene(void);

	/********************************************************************************************
	Summary:	Runs the program and starts the MainLoop.
				Returns the exit code.
	Parameters: None.
	Returns:	UINT.
	********************************************************************************************/
	UINT run(void);

	/********************************************************************************************
	Summary:	Runs the program and starts the MainLoop.
				Returns the exit code.
	Parameters: [out] retval - exit code.
	********************************************************************************************/
	void run(UINT *retval);

	/********************************************************************************************
	Summary:	Translates WORLD matrix.
	Parameters: [in] v - translation vector.
	********************************************************************************************/
	void translate(Vector v);

	/********************************************************************************************
	Summary:	Scales WORLD matrix.
	Parameters: [in] v - scaling vector.
	********************************************************************************************/
	void scale(Vector v);

	/********************************************************************************************
	Summary:	Scales WORLD matrix.
	Parameters: [in] f - scaling float.
	********************************************************************************************/
	void scale(float f);

	/********************************************************************************************
	Summary:	rotates WORLD matrix through Yaw, Pitch and Roll.
	Parameters: [in] v - rotation vector.
	********************************************************************************************/
	void rotateYPR(Vector v);

	/********************************************************************************************
	Summary:	rotates WORLD matrix.
	Parameters: [in] v - axis vector.
				[in] f - angle.
	********************************************************************************************/
	void rotateAxis(Vector v, float f);

	/********************************************************************************************
	Summary:	Pushes the WORLD matrix into the matrix stack.
	Parameters: None.
	********************************************************************************************/
	void pushMatrix(void);

	/********************************************************************************************
	Summary:	Pops the WORLD matrix out of the matrix stack.
	Parameters: None.
	********************************************************************************************/
	void popMatrix(void);

	/********************************************************************************************
	Summary:	Multiplies a matrix by the world matrix.
	Parameters: [in] mat - matrix to multiply by.
	********************************************************************************************/
	void multiplyMatrixWithWorld(D3DXMATRIX *mat);

	/********************************************************************************************
	Summary:	The UI Vertex Decleration.
	********************************************************************************************/
	struct VERTEX2D
	{
		//UI_FVF
		//pos
		float X;
		float Y;
		float Z;
		float RHW;
		//tex
		float U;
		float V;
		static D3DVERTEXELEMENT9* GetDeclarationElements(void)
		{
			static D3DVERTEXELEMENT9 decl[] = 
			{
				{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
				{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				D3DDECL_END()
			};
			return decl;
		}
	};

	/********************************************************************************************
	Summary:	The 3D (or Game mode) Vertex Decleration.
	********************************************************************************************/
	struct VERTEX3D
	{
		//pos
		float pX;
		float pY;
		float pZ;
		//norm
		float nX;
		float nY;
		float nZ;
		//tex1
		float u;
		float v;
		//tangent
		float tX;
		float tY;
		float tZ;
		//binormal
		float bX;
		float bY;
		float bZ;
		////bone indices
		//UCHAR mBoneIndices[4];
		////bone weights
		//UCHAR mBoneWeights[4];
		static D3DVERTEXELEMENT9* GetDeclarationElements(void)
		{
			static D3DVERTEXELEMENT9 decl[] = 
			{
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
				{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
				{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
				/*{ 0, 68, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
				{ 0, 72, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },*/
				D3DDECL_END()
			};
			return decl;
		}
	};

	/********************************************************************************************
	Summary:	Sets the current Vertex Decleration to the UI.
	Parameters: None.
	********************************************************************************************/
	void setUIVertDecl(void);

	/********************************************************************************************
	Summary:	Sets the current Vertex Decleration to the Game or 3D.
	Parameters: None.
	********************************************************************************************/
	void setGameVertDecl(void);

	/********************************************************************************************
	Summary:	Sets the current Vertex Decleration to the UI.
	Parameters: [in] key - keycode to check if pressed. e.g. (VK_SHIFT or 'W')
	********************************************************************************************/
	//bool isKeyDown(USHORT key);

	/********************************************************************************************
	Summary:	Get the current mouse state.
	Parameters: None.
	Returns:	MOUSESTATE
	********************************************************************************************/
	//MOUSESTATE getMouseState(void);
	Input* getInput(void);

	/********************************************************************************************
	Summary:	Exits the current program with a code
	Parameters: [in] code - exit code.
	********************************************************************************************/
	void quitProgram(int code);

	/********************************************************************************************
	Summary:	Tells the renderer to take a screenshot of the next frame.
	Parameters: None.
	********************************************************************************************/
	void screenshot(void);

	/********************************************************************************************
	Summary:	Returns the Havok Physics World.
	Parameters: None.
	Returns:	hkpWorld*.
	********************************************************************************************/
	hkpWorld* getHKWorld(void);

	/********************************************************************************************
	Summary:	This updates the world view projection matrix for the lighting shader.
	Parameters: None.
	********************************************************************************************/
	void updateLighting(void);

	/********************************************************************************************
	Summary:	Returns the Lighting Effect.
	Parameters: None.
	Returns:	ID3DXEffect*.
	********************************************************************************************/
	ID3DXEffect* getLightingEffect(void);

	/********************************************************************************************
	Summary:	Quits the Havok modules. This should be placed as the last line in main, 
				before the return statement
	Parameters: None.
	********************************************************************************************/
	void killHavok(void);

private:

	static DirectXRenderer* pInstance;					//Instance of the renderer
	static Destroyer<DirectXRenderer> destroyer;		//Singleton destroyer
	static LRESULT CALLBACK WndProc(HWND hWnd,			//Windows callback function
									UINT message, 
									WPARAM wParam, 
									LPARAM lParam);

	IDirect3DDevice9* pd3dDev;							//Direct3DDevice
	LPDIRECT3D9 lpd3d;									//Direct3D
	IDirect3DVertexDeclaration9* pd3dGAMEVDecl;			//Game(3D) Vertex Decleration
	IDirect3DVertexDeclaration9* pd3dUIVDecl;			//UI(2D) Vertex Decleration
	D3DCAPS9 d3dCaps;									//Device capabilities
	//bool keyCode[256];									//Keyboard character codes
	bool bQuit;											//quit BOOL
	//MOUSESTATE mouseState;								//Stores current RAW mouse state
	Input* pInput;
	std::vector<D3DXMATRIX> vMatrixStack;				//Matrix stack for D3D
	UINT16 wWidth, wHeight;								//Width and height of the window
	HWND hWnd;											//Window class handle
	Scene* activeScene;									//Scene that will be rendering
	//RTT
	LPDIRECT3DTEXTURE9 pRenderTexture;					//'Render to texture' texture
	LPDIRECT3DSURFACE9 pRenderSurface, pBackBuffer;		//Render surfaces for RTT
	LPDIRECT3DVERTEXBUFFER9 pRenderVB;					//The full screen square that 
														//we will be rendering our texture to.
	ID3DXEffect *pPostProcess, *pLighting;				//Shader effect handles
	bool bSS;											//Screenshot boolean
	//ShadowMap (later)
	//LPDIRECT3DTEXTURE9 pShadowMap;						//Shadow Map Texture
	//LPDIRECT3DSURFACE9 pShadowSurf;						//Shadow Map Surface
	//Havok
	hkpWorld* world;									//Havok Physics World
	hkMemoryRouter* memoryRouter;						//Havok Memory handler
	hkMallocAllocator baseMalloc;						//Havok Memory allocator
#ifdef _DEBUG
	hkVisualDebugger* visualDebugger;					//Havok Physics Visual Debugger
#endif
	hkpPhysicsContext* physicsContext;					//Havok Visual Debbuger physics context
	//time
	LARGE_INTEGER nStopTime, nStartTime, nFreq;			//for calculating FPS
	int SSNum;

	/********************************************************************************************
	Summary:	This is an error report function passed to the Havok System.
	********************************************************************************************/
	static void HK_CALL havokErrorReport(const char* msg, void* userContext)
	{
		LogManager::getInstance()->trace("[HAVOK] %s", msg);
	}

	/********************************************************************************************
	Summary:	Initializes the Havok Base System.
	Parameters: None.
	********************************************************************************************/
	void initHavok(void);

	/********************************************************************************************
	Summary:	Initializes the Havok Physics System.
	Parameters: None.
	********************************************************************************************/
	void initHavokPhysics(void);

	/********************************************************************************************
	Summary:	Initializes Keyboard and Mouse Input. (using RAW input)
	Parameters: None.
	********************************************************************************************/
	//void initRAWInput(void);

	/********************************************************************************************
	Summary:	This is the actual screen capture function.
	Parameters: None.
	********************************************************************************************/
	void internalScreenCap(void);

	/********************************************************************************************
	Summary:	Begin Rendering.
	Parameters: None.
	********************************************************************************************/
	void beginRender(void);

	/********************************************************************************************
	Summary:	Finish Rendering.
	Parameters: None.
	********************************************************************************************/
	void endRender(void);

	/********************************************************************************************
	Summary:	This is the Main loop.
	Parameters: None.
	Returns:	WPARAM.
	********************************************************************************************/
	WPARAM MainLoop(void);

	/********************************************************************************************
	Summary:	Default Constructor.
	Parameters: None.
	********************************************************************************************/
	DirectXRenderer(void);

	/********************************************************************************************
	Summary:	Default Destructor.
	Parameters: None.
	********************************************************************************************/
	virtual ~DirectXRenderer(void);
};

#endif //__DXRENDER_H_