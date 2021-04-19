#ifndef __RENDERABLE_H_
#define __RENDERABLE_H_

class Renderable
{
	friend class Scene;
protected:
	bool bToDie;
public:
	virtual void Render(void) = 0;
	virtual void Update(double dTime) = 0;
	void setToDie(void){bToDie=true;}
	Renderable(void){bToDie=false;}
	virtual ~Renderable(void){}
};

#endif //__RENDERABLE_H_