#pragma once

#include <vector>
#include <set>
#include "Enemy.h"
#include <DirectXMath.h>

// �G�l�~�[�}�l�[�W���[
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager();

public:
    // �B��̃C���X�^���X�擾
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    // �X�V����
    void Update(float elapsedTime);
    void UpdateOnlyTransform(float elapsedTime);

    // �`�揈��
    void Render(const RenderContext& rc, ModelShader* shader);

    // �G�l�~�[�o�^
    void Register(Enemy* enemy);

    // �G�l�~�[�폜
    void Remove(Enemy* enemy);

    // �G�l�~�[�S�폜
    void Clear();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    void DrawDebugGUI();

    // �G�l�~�[���擾
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }
    int GetCurrentEnemyCount() const { return currentEnemyCount; }
    void SetCurrentEnemyCount(int co) { currentEnemyCount = co; }

    // �G�l�~�[�擾
    Enemy* GetEnemy(int index) { return enemies.at(index); }



    // �t�^����ID����G�l�~�[���擾
    Enemy* GetEnemyFromId(int id);

private:
    // �v���C���[�ƃG�l�~�[�̏Փˏ���
    void CollisionEnemyVsEnemies();

private:
    std::vector<Enemy*> enemies;
    std::set<Enemy*>    removes;

public:
    int identity = 0;	// �t�^����ID�̒l(���̒l��MetaAI::Identity::Enemy�����Z���ĕt�^����)

    int currentEnemyCount = 0;

};



