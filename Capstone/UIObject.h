#ifndef __UIOBJECT_H_
#define __UIOBJECT_H_

#include "DirectXRenderer.h"
#include "Renderable.h"

class UIObject : public Renderable
{
private:
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DVERTEXBUFFER9 pVB;
public:
	UIObject(const char* texFile, int width, int height, int x, int y, int zOrder=0);
	void Render(void);
	void Update(double dTime);
	virtual ~UIObject(void);
};

#endif //__UIOBJECT_H_