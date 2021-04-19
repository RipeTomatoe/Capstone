#include "UIObject.h"

UIObject::UIObject(const char* texFile, int width, int height, int x, int y, int zOrder)
{
	char file[255];
	sprintf_s(file, "Resources/Textures/%s", texFile);

	DirectXRenderer::VERTEX2D temp[] = {{(float)x, (float)y, (float)zOrder, 1.0f, 0.0f, 0.0f},
									 {(float)(width + x), (float)y, (float)zOrder, 1.0f, 1.0f, 0.0f},
									 {(float)x, (float)(height + y), (float)zOrder, 1.0f, 0.0f, 1.0f},
									 {(float)(width + x), (float)(height + y), (float)zOrder, 1.0f, 1.0f, 1.0f}};

	DirectXRenderer::getInstance()->getDevice()->CreateVertexBuffer(	
		sizeof(temp),
		D3DUSAGE_WRITEONLY,
		NULL,
		D3DPOOL_MANAGED,
		&pVB,
		NULL);
	VOID* pVoid;
	pVB->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, temp, sizeof(temp));
	pVB->Unlock();
	D3DXCreateTextureFromFile(DirectXRenderer::getInstance()->getDevice(),
							file,
							&pTexture);
}

void UIObject::Update(double dTime)
{
	
}

void UIObject::Render(void)
{
	DirectXRenderer::getInstance()->setUIVertDecl();

	DirectXRenderer::getInstance()->getDevice()->SetTexture(0, pTexture);
	DirectXRenderer::getInstance()->getDevice()->SetStreamSource(0, pVB, 0, sizeof(DirectXRenderer::VERTEX2D));
	DirectXRenderer::getInstance()->getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

	//terminate texture
	DirectXRenderer::getInstance()->getDevice()->SetTexture(0, NULL);
}

UIObject::~UIObject(void)
{
	pTexture->Release();
	pTexture = NULL;
	pVB->Release();
	pVB = NULL;
}