#include "Effect.h"

Effect::Effect(const char* fileName)
{
	char temp[255];
	sprintf_s(temp, "Resources/Shaders/%s", fileName);
	ID3DXBuffer* buffer;
	if(FAILED(D3DXCreateEffectFromFile(DirectXRenderer::getInstance()->getDevice(), temp, /*macros*/NULL, NULL, 0, NULL, &effect, &buffer)))
	{
		if(buffer)
		{
			LogManager::getInstance()->trace("[ERROR] HLSL: %s", buffer->GetBufferPointer());
			buffer->Release();
		}
		LogManager::getInstance()->trace("Unable to compile effect: %s", fileName);
	}
}