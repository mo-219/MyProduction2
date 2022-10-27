#pragma once
#include <vector>
#include "Stage.h"

// �X�e�[�W�}�l�[�W���[
class StageManager
{
private:
    StageManager() {}
    ~StageManager() {}

public:
    static StageManager& Instance()
    {
        static StageManager instance;
        return instance;
    }

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Render(RenderContext& rc, ModelShader* shader);

    // ���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    // �X�e�[�W�o�^
    void Register(Stage* stage);

    // �X�e�[�W�S�폜
    void Clear();

    // �X�e�[�W���Q�b�^�[
    int GetStageCount() const { return static_cast<int>(stages.size()); }

    // �G�l�~�[�擾
    Stage* GetStage(int index) { return stages.at(index); }

    // �f�o�b�O�pGUI�`��
    void DrawDebugGUI();

    // �����蔻��̕`��
    void DrawDebugPrimitive();


private:
    std::vector<Stage*> stages;

};