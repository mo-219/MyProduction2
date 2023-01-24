#include <imgui.h>
#include "StageManager.h"
#include "Graphics/Graphics.h"

void StageManager::Update(float elapsedTime)
{
    for (Stage* stage : stages)
    {
        stage->Update(elapsedTime);
    }
    // 破棄処理
    for (Stage* stage : removes)
    {
        std::vector<Stage*>::iterator it =
            std::find(stages.begin(), stages.end(), stage);

        if (it != stages.end())
        {
            stages.erase(it);
        }
        // 弾丸の破棄処理
        delete stage;
    }

    // 破棄リストをクリア
    removes.clear();

}

void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Stage* stage : stages)
    {
        stage->Render(dc, shader);
    }
}
void StageManager::Render(RenderContext& rc, ModelShader* shader)
{
    for (Stage* stage : stages)
    {
        RenderContext uniqueRc = stage->SetRenderContext(rc);
        stage->Render(uniqueRc, shader);
    }
}


bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;

    hit.distance = FLT_MAX;
    for (Stage* stage : stages)
    {
        HitResult tmp;
        if (stage->RayCast(start, end, tmp))
        {
            if (hit.distance > tmp.distance)
            {
                hit = tmp;
                result = true;
            }
        }
    }

    return result;
}

void StageManager::Register(Stage* stage)
{
    stages.emplace_back(stage);
}

void StageManager::Remove(Stage* stage)
{
    // 破棄リストに追加
    removes.insert(stage);
}

void StageManager::Clear()
{
    for (Stage* stage : stages)
    {
        if (stage != nullptr)
        {
            delete stage;
            stage = nullptr;
        }
    }
    stages.clear();

}

void StageManager::StageNumDelete(int type)
{
    for (Stage* stage : stages)
    {
        if (stage->GetStageNum() == type)
        {
            stage->Destroy();
        }
    }
}

void StageManager::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(900, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Stage", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (int i = 0; i < GetStageCount(); i++)
            {
                Stage* stage = GetStage(i);

                // 位置
                DirectX::XMFLOAT3 value = stage->GetPosition();
                ImGui::InputFloat3("position", &value.x);


            }

        }
    }
    ImGui::End();
}

void StageManager::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();


    // 衝突判定用のデバッグ円柱を描画
    for (Stage* stage : stages)
    {
        if (stage->GetStageNum() != StageNumber::Door) continue;        // ドアじゃなかったら判定無し   

        DirectX::XMFLOAT3 pos = stage->GetPosition();
        pos = DirectX::XMFLOAT3(pos.x - stage->GetWidth() / 2, pos.y +1, pos.z);

        if (!stage->GetCollisionFlag())
        {
            debugRenderer->DrawCube(pos, stage->GetWidth(), stage->GetHeight(), stage->GetDepth(), DirectX::XMFLOAT4(1, 0, 0, 0));
        }
        else
        {
            debugRenderer->DrawCube(pos, stage->GetWidth(), stage->GetHeight(), stage->GetDepth(), DirectX::XMFLOAT4(1, 0, 0, 1));
        }
    }

}
