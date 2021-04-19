#ifndef __SCENE_H_
#define __SCENE_H_

#include <vector>
#include <list>
#include <Windows.h>
#include "Renderable.h"

class Scene
{
	friend class Camera;
	typedef std::list<Renderable*> VectorType;
protected:
	VectorType objToRender;
	VectorType UIToRender;
	void RenderObjects(void)
	{
		for(VectorType::iterator i = objToRender.begin(); i != objToRender.end();)
		{
			if((*i)->bToDie)
			{
				delete (*i);
				i = objToRender.erase(i);
			}
			else
			{
				(*i)->Render();
				++i;
			}
		}
	}
	void RenderUI(void)
	{
		for(VectorType::iterator i = UIToRender.begin(); i != UIToRender.end();)
		{
			if((*i)->bToDie)
			{
				delete (*i);
				i = UIToRender.erase(i);
			}
			else
			{
				(*i)->Render();
				++i;
			}
		}
	}
	void UpdateObjects(double dTime)
	{
		for(VectorType::iterator i = objToRender.begin(); i != objToRender.end();)
		{
			if((*i)->bToDie)
			{
				delete (*i);
				i = objToRender.erase(i);
			}
			else
			{
				(*i)->Update(dTime);
				++i;
			}
		}

		for(VectorType::iterator i = UIToRender.begin(); i != UIToRender.end();)
		{
			if((*i)->bToDie)
			{
				delete (*i);
				i = UIToRender.erase(i);
			}
			else
			{
				(*i)->Update(dTime);
				++i;
			}
		}
	}
public:
	bool is3D;
	bool isLoading;
	unsigned int currentLevel;
	Scene(void){is3D = false;currentLevel=1;isLoading=true;}
	virtual ~Scene(void)
	{
		Sleep(15);
		for(VectorType::iterator i = objToRender.begin(); i != objToRender.end();)
		{
			delete (*i);
			i = objToRender.erase(i);
		}

		for(VectorType::iterator i = UIToRender.begin(); i != UIToRender.end();)
		{
			delete (*i);
			i = UIToRender.erase(i);
		}
		objToRender.clear();
		UIToRender.clear();
	}
	virtual void Render(void) = 0;
	virtual void Update(double dTime) = 0;
};

#endif //__SCENE_H_