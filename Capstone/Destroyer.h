#ifndef __DESTROYER_H_
#define __DESTROYER_H_

template<class T>
class Destroyer
{
public:
	/********************************************************************************************
	Summary:	Default Constructor
	Parameters: [in] dest - The pointer of the singleton to be destroyed.
	********************************************************************************************/
	Destroyer(T* dest = 0)
	{
		pToDie = dest;
	}

	/********************************************************************************************
	Summary:	Default Destructor
	Parameters: None.
	********************************************************************************************/
	virtual ~Destroyer(void)
	{
		delete pToDie;
	}

	/********************************************************************************************
	Summary:	Sets the singleton to be destroyed
	Parameters: [in] dest - The pointer of the singleton to be destroyed.
	********************************************************************************************/
	void setSingleton(T* dest)
	{
		pToDie = dest;
	}
private:
	T* pToDie;
};

#endif //__DESTROYER_H_