#include"StageMain.h"

//static Stage* instance = nullptr;
//
//// �C���X�^���X�擾
//Stage& Stage::Instance()
//{
//    return *instance;
//}


// �R���X�g���N�^
StageMain::StageMain()
{
    //�X�e�[�W���f���ǂݍ���
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

// �f�X�g���N�^
StageMain::~StageMain()
{
    // �X�e�[�W���f����j��
    delete model;
}

// �X�V����
void StageMain::Update(float elapsedTime)
{
    //���͂�邱�ƂȂ�

}

// �`�揈��
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model);
}

void StageMain::Render(const RenderContext& rc, ModelShader* shader)
{
    shader->Draw(rc, model);
}

RenderContext StageMain::SetRenderContext(const RenderContext& rc)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;

    return myRc;
}

// ���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}