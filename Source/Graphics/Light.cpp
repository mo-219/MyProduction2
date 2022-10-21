#include "Graphics/Graphics.h"
#include "Light.h"

Light::Light(LightType lightType)
    : lightType(lightType)
{
}

void Light::PushRenderContext(RenderContext& rc) const
{
    // ìoò^Ç≥ÇÍÇƒÇ¢ÇÈåıåπÇÃèÓïÒÇê›íË
    switch (lightType)
    {
    case LightType::Directional:
        rc.directionalLightData.direction.x = direction.x;
        rc.directionalLightData.direction.y = direction.y;
        rc.directionalLightData.direction.z = direction.z;
        rc.directionalLightData.direction.w = 0.0f;
        rc.directionalLightData.color = color;

        break;
    }
}


void Light::DrawDebugGUI()
{
    static constexpr char* lightTypeName[] =
    {
        "Directional",
        "Point",
    };

    if (ImGui::TreeNode(lightTypeName[static_cast<int>(lightType)]))
    {
        switch (lightType)
        {
        case LightType::Directional:
            {
                if (ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f))
                {
                    float l = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z *
                        direction.z);
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
    // åªèÛÇÕì¡Ç…âΩÇ‡ÇµÇ»Ç¢
}
