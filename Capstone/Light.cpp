#include "Light.h"

int Light::lightCount = 0;

Light::Light(Vector pos, COLOUR_VALUE amb, COLOUR_VALUE diff, COLOUR_VALUE spec, float range)
{
	Range = range;
	Position = pos;
	Diffuse.SetValues(diff);
	Ambient.SetValues(amb);
	Specular.SetValues(spec);
	lightCount++;
}

void Light::Update(void)
{
	DirectXRenderer::getInstance()->getLightingEffect()->SetFloat("LightRange", Range);
	DirectXRenderer::getInstance()->getLightingEffect()->SetVector("LightPosition", &Position.dxGetVector4());
	DirectXRenderer::getInstance()->getLightingEffect()->SetVector("DiffuseColor", &Diffuse.dxGetVector4());
	DirectXRenderer::getInstance()->getLightingEffect()->SetVector("SpecularColor", &Specular.dxGetVector4());
	DirectXRenderer::getInstance()->getLightingEffect()->SetVector("AmbientColor", &Ambient.dxGetVector4());
}