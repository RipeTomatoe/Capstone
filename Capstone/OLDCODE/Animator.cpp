#include "Animator.h"

Animator::Animator(const aiScene* pScene, size_t pAnimIndex)
{
	mScene = pScene;
	mCurrentAnimIndex = -1;
	mAnimEvaluator = NULL;
	mRootNode = NULL;

	// build the nodes-for-bones table
	for (unsigned int i = 0; i < pScene->mNumMeshes;++i)
	{
		const aiMesh* mesh = pScene->mMeshes[i];
		for (unsigned int n = 0; n < mesh->mNumBones;++n)
		{
			const aiBone* bone = mesh->mBones[n];

			mBoneNodesByName[bone->mName.data] = pScene->mRootNode->FindNode(bone->mName);
		}
	}

	// changing the current animation also creates the node tree for this animation
	SetAnimIndex( pAnimIndex);
}

Animator::~Animator(void)
{
	delete mRootNode;
	delete mAnimEvaluator;
}

void Animator::SetAnimIndex(size_t pAnimIndex)
{
	if(pAnimIndex == mCurrentAnimIndex)
		return;

	//kill data of the previous anim
	delete mRootNode;
	mRootNode = NULL;
	delete mAnimEvaluator;
	mAnimEvaluator = NULL;
	mNodesByName.clear();

	mCurrentAnimIndex = pAnimIndex;
	mRootNode = CreateNodeTree(mScene->mRootNode, NULL);

	//INVALID!
	if(mCurrentAnimIndex >= mScene->mNumAnimations)
		return;

	mAnimEvaluator = new AnimEvaluator(mScene->mAnimations[mCurrentAnimIndex]);
}

void Animator::Calculate(double pTime)
{
	if(!mAnimEvaluator)
		return;

	mAnimEvaluator->Evaluate(pTime);

	UpdateTransforms(mRootNode, mAnimEvaluator->GetTransformations());
}

const aiMatrix4x4& Animator::GetLocalTransform(const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find(node);
	if(it == mNodesByName.end())
		return mIdentityMatrix;

	return it->second->mLocalTransform;
}

const aiMatrix4x4& Animator::GetGlobalTransform(const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find(node);
	if(it == mNodesByName.end())
		return mIdentityMatrix;

	return it->second->mGlobalTransform;
}

const std::vector<aiMatrix4x4>& Animator::GetBoneMatrices(const aiNode* node, size_t pMeshIndex)
{
	ai_assert(pMeshIndex < node->mNumMeshes);
	size_t meshIndex = node->mMeshes[pMeshIndex];
	ai_assert(meshIndex < mScene->mNumMeshes);
	const aiMesh* mesh = mScene->mMeshes[meshIndex];

	mTransforms.resize(mesh->mNumBones, aiMatrix4x4());

	aiMatrix4x4 globalInverseMeshTransform = GetGlobalTransform(node);
	globalInverseMeshTransform.Inverse();

	for( size_t i = 0; i < mesh->mNumBones; ++i)
	{
		const aiBone* bone = mesh->mBones[i];
		const aiMatrix4x4& currentGlobalTransform = GetGlobalTransform( mBoneNodesByName[ bone->mName.data ]);
		mTransforms[i] = globalInverseMeshTransform * currentGlobalTransform * bone->mOffsetMatrix;
	}

	return mTransforms;
}

SceneAnimNode* Animator::CreateNodeTree(aiNode* node, SceneAnimNode* parent)
{
	SceneAnimNode* internalNode = new SceneAnimNode(node->mName.data);
	internalNode->mParent = parent;
	mNodesByName[node] = internalNode;

	internalNode->mLocalTransform = node->mTransformation;
	CalculateGlobalTransform(internalNode);

	if(mCurrentAnimIndex < mScene->mNumAnimations)
	{
		internalNode->mChannelIndex = -1;
		const aiAnimation* currentAnim = mScene->mAnimations[mCurrentAnimIndex];
		for(size_t i = 0; i < currentAnim->mNumChannels; ++i)
		{
			if(currentAnim->mChannels[i]->mNodeName.data == internalNode->mName)
			{
				internalNode->mChannelIndex = i;
				break;
			}
		}
	}

	for(size_t i = 0; i < node->mNumChildren; ++i)
	{
		SceneAnimNode* childNode = CreateNodeTree(node->mChildren[i], internalNode);
		internalNode->mChildren.push_back(childNode);
	}

	return internalNode;
}

void Animator::UpdateTransforms(SceneAnimNode* node, const std::vector<aiMatrix4x4>& pTransforms)
{
	if(node->mChannelIndex != -1)
	{
		ai_assert(node->mChannelIndex < pTransforms.size());
		node->mLocalTransform = pTransforms[node->mChannelIndex];
	}

	CalculateGlobalTransform(node);

	for(std::vector<SceneAnimNode*>::iterator it = node->mChildren.begin(); it != node->mChildren.end(); ++it)
		UpdateTransforms(*it, pTransforms);
}

void Animator::CalculateGlobalTransform(SceneAnimNode* pInternalNode)
{
	pInternalNode->mGlobalTransform = pInternalNode->mLocalTransform;
	SceneAnimNode* node = pInternalNode->mParent;
	while(node)
	{
		pInternalNode->mGlobalTransform = node->mLocalTransform * pInternalNode->mGlobalTransform;
		node = node->mParent;
	}
}