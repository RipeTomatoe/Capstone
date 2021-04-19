#ifndef __ANIMATOR_H_
#define __ANIMATOR_H_

#include <map>
#include <vector>
#include "AnimEvaluator.h"
#include "assimp\include\assimp.h"
#include "assimp\include\aiAnim.h"
#include "assimp\include\aiScene.h"

struct SceneAnimNode
{
	std::string mName;
	SceneAnimNode* mParent;
	std::vector<SceneAnimNode*> mChildren;

	//most recently calculated local transform
	aiMatrix4x4 mLocalTransform;

	//same, but in world space
	aiMatrix4x4 mGlobalTransform;

	//index in the current animation's channel array. -1 if not animated.
	size_t mChannelIndex;

	//default construction
	SceneAnimNode(void)
	{
		mChannelIndex = -1;
		mParent = NULL;
	}

	//Construction from a given name
	SceneAnimNode(const std::string& pName)
		: mName(pName)
	{
		mChannelIndex = -1;
		mParent = NULL;
	}

	//Destruct all children recursuively
	~SceneAnimNode(void)
	{
		for(std::vector<SceneAnimNode*>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
			delete *it;
	}
};

class Animator
{
public:
	Animator(const aiScene* pScene, size_t pAnimIndex = 0);
	~Animator();
	void SetAnimIndex(size_t pAnimIndex);
	void Calculate(double pTime);
	const aiMatrix4x4& GetLocalTransform(const aiNode* node) const;
	const aiMatrix4x4& GetGlobalTransform(const aiNode* node) const;
	const std::vector<aiMatrix4x4>& GetBoneMatrices(const aiNode* node, size_t pMeshIndex = 0);
	size_t CurrentAnimIndex(void) const { return mCurrentAnimIndex; }
	aiAnimation* CurrentAnim(void) const { return (mCurrentAnimIndex < mScene->mNumAnimations ? mScene->mAnimations[mCurrentAnimIndex] : NULL); }
protected:
	SceneAnimNode* CreateNodeTree(aiNode* node, SceneAnimNode* parent);
	void UpdateTransforms(SceneAnimNode* node, const std::vector<aiMatrix4x4>& pTransforms);
	void CalculateGlobalTransform(SceneAnimNode* pInternalNode);

	const aiScene* mScene;
	size_t mCurrentAnimIndex;
	AnimEvaluator* mAnimEvaluator;
	SceneAnimNode* mRootNode;
	typedef std::map<const aiNode*, SceneAnimNode*> NodeMap;
	NodeMap mNodesByName;
	typedef std::map<const char*, const aiNode*> BoneMap;
	BoneMap mBoneNodesByName;
	std::vector<aiMatrix4x4> mTransforms;
	aiMatrix4x4 mIdentityMatrix;
};

#endif //__ANIMATOR_H_