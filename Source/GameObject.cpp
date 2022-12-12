#include "GameObject.h"
#include <imgui.h>

GameObject::~GameObject()
{
    delete model;
}

void GameObject::CreateModel(const char* filename)
{
    model = new Model(filename);
}

void GameObject::Update(float elapsedTime)
{
    UpdateTransform();
    model->UpdateTransform(transform);
}

void GameObject::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void GameObject::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}

void GameObject::UpdateTransform()
{

    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);


    // 回転行列を作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);


    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(param.position.x, param.position.y, param.position.z);


    // ３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;    //左手系


    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

void GameObject::UpdateOnlyTransform(float elapsedTime)
{
}

RenderContext GameObject::SetRenderContext(const RenderContext& rc)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;
    myRc.heightFogFlag = heightFogFlag;
    if (heightFogFlag == 0)
    {
        myRc = myRc;
    }

    return myRc;
}
void GameObject::DrawDebugImGui()
{
    ImGui::Separator();
    char* str = (char*)code.c_str();
    ImGui::InputText("code", str, sizeof(str));
    if(CollisionFlag)    ImGui::Text("true");
    else                 ImGui::Text("false");

    ImGui::InputFloat3("position", &param.position.x);
    ImGui::InputFloat3("scale", &scale.x);
    ImGui::SliderFloat3("angle", &angle.x, 0.0f,10.0f);

    ImGui::SliderFloat("radius", &param.radius, 0.1f, 10.0f);

    ImGui::SliderFloat("width", &param.width, 0.1f, 10.0f);
    ImGui::SliderFloat("height", &param.height, 0.1f, 10.0f);
    ImGui::SliderFloat("depth", &param.depth, 0.1f, 10.0f);

    
    char co[255]{};
    strcpy_s(co, code.c_str());
    ImGui::InputText("code", co, sizeof(co));

}

void GameObject::Initialize()
{
    scale.x *= 0.012f;
    scale.y *= 0.012f;
    scale.z *= 0.012f;
}
