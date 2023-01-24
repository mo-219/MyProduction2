#include "EnemyManager.h"
#include "Graphics/Graphics.h"

#include "Collision.h"

#include "MetaAI.h"

EnemyManager::~EnemyManager()
{
    removes.clear();
    enemies.clear();
    
}

// �X�V����
void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    // �j������
    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it =
            std::find(enemies.begin(), enemies.end(), enemy);

        if (it != enemies.end())
        {
            enemies.erase(it);
        }
        // �e�ۂ̔j������
        delete enemy;
    }
    // �j�����X�g���N���A
    removes.clear();

    // �G���m�̏Փ˔���
    CollisionEnemyVsEnemies();
}

void EnemyManager::UpdateOnlyTransform(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->UpdateOnlyTransform(elapsedTime);
    }
}

// �`�揈��
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(context, shader);
    }
}
void EnemyManager::Render(const RenderContext& rc, ModelShader* shader)
{
    for (Enemy* enemy : enemies)
    {
        RenderContext myRc = enemy->SetRenderContext(rc);
        enemy->Render(myRc, shader);
    }
}

// �G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
    enemy->SetId(static_cast<int>(Meta::Identity::Enemy) + identity++);
    enemies.emplace_back(enemy);
}

// �G�l�~�[�S�폜
void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        if (enemy != nullptr)
        {
            delete enemy;
            enemy = nullptr;
        }
    }
    enemies.clear();
}

void EnemyManager::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();


    // �Փ˔���p�̃f�o�b�O�~����`��
    for (Enemy* enemy : enemies)
    {
        debugRenderer->DrawCylinder(enemy->GetPosition(), enemy->GetRadius(),enemy->GetHeight(), DirectX::XMFLOAT4(1, 0, 0, 1));
        enemy->DrawDebugPrimitive();
    }

}

void EnemyManager::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_None))
    {
        for (Enemy* enemy : enemies)
        {
            enemy->DrawDebugImGui();
        }
        int a = GetEnemyCount();
        ImGui::InputInt("EnemyCount", &a);
    }
    ImGui::End();

}

Enemy* EnemyManager::GetEnemyFromId(int id)
{
    for (Enemy* enemy : enemies)
    {
        if (enemy->GetId() == id)
            return enemy;
    }
    return nullptr;
}

void EnemyManager::CollisionEnemyVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    int enemyCount = enemyManager.GetEnemyCount();
    for (int j = 0; j < enemyCount; ++j)
    {
        Enemy* enemy1 = enemyManager.GetEnemy(j);

        for (int i = j+1; i < enemyCount; ++i)
        {
            if (i == j)  continue;

            DirectX::XMFLOAT3 outPosition{};
            Enemy* enemy2 = enemyManager.GetEnemy(i);


            if (Collision::InstersectCylinderVsCylinder(enemy1->GetPosition(), enemy1->GetRadius(),enemy1->GetHeight(),
                                                    enemy2->GetPosition(), enemy2->GetRadius(), enemy2->GetHeight(),
                                                    outPosition))
            {
                // �����o����̈ʒu�ݒ�
                enemyManager.GetEnemy(i)->SetPosition({ outPosition.x,
                                                        outPosition.y,
                                                        outPosition.z });

            }
        }
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    // �j�����X�g�ɒǉ�
    removes.insert(enemy);
}

