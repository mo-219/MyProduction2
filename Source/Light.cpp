#include	"Graphics/Graphics.h"
#include	"Light.h"

Light::Light(LightType lightType)
	: lightType(lightType)
{
}

void Light::DrawDebugGUI()
{
	static constexpr char* lightTypeName[] =
	{
		"Directional",
	};

	if (ImGui::TreeNode(lightTypeName[static_cast<int>(lightType)]))
	{
		switch (lightType)
		{
		case	LightType::Directional:
		{
			if (ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f))
			{
				float l = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
				direction.x /= l;
				direction.y /= l;
				direction.z /= l;
			}
			ImGui::ColorEdit3("color", &color.x);
			break;
		}
		}
		ImGui::TreePop();
	}
}

void Light::DrawDebugPrimitive()
{
	//	åªèÛÇÕì¡Ç…âΩÇ‡ÇµÇ»Ç¢
}
