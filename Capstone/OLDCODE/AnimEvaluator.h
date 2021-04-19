#ifndef __ANIMEVAL_H_
#define __ANIMEVAL_H_

#include "assimp\include\assimp.h"
#include "assimp\include\aiAnim.h"
#include "assimp\code\BoostWorkaround\boost\tuple\tuple.hpp"
#include <vector>

class AnimEvaluator
{
public:
	AnimEvaluator(const aiAnimation* pAnim);
	void Evaluate(double pTime);
	const std::vector<aiMatrix4x4>& GetTransformations(void) const { return mTransforms; }
protected:
	const aiAnimation* mAnim;
	double mLastTime;
	std::vector<boost::tuple<size_t, size_t, size_t> > mLastPositions;
	std::vector<aiMatrix4x4> mTransforms;
};

#endif //__ANIMEVAL_H_