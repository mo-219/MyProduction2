#include "ItemObj.h"
#include "Player.h"

ItemHeel::ItemHeel(int i)
{
    heelPoint = i;
    model = new Model("Data/Player/Cross.mdl");
}

ItemHeel::ItemHeel()
{
    Audio& audio = Audio::Instance();
    se = audio.LoadAudioSource("Data/Audio/SE/HP.wav");

    scale = { 0.5f,0.5f,0.5f };
    code = "HeelItem";
    // ‚ ‚½‚è”»’èÝ’è
    setRayCast(false);
    setCollision(CollisionType::Sphere);
    param.radius = 0.5;

    model = new Model("Data/Model/Cross/Cross.mdl");

    invincibleTimer = 0.6f;
}

ItemHeel::~ItemHeel()
{
    delete model;
}


void ItemHeel::Hit(Character* chara, DirectX::XMFLOAT3 out)
{
    if (chara->GetName() != Character::CharacterName::PLAYER) return;

    int ActualHP = chara->GetHealth() + heelPoint;
    if (ActualHP >= chara->GetMaxHealth()) chara->SetHealth(chara->GetMaxHealth());
    else                                   chara->SetHealth(ActualHP);

    se->Play(false);

    Destroy(); 
}

void ItemHeel::Update(float elapsedTime)
{
    param.position.y += sinf(timer*elapsedTime * 3.0f)/55.0f;
    angle.y += DirectX::XMConvertToRadians(2);
    if (angle.y >= 2 * DirectX::XM_PI)
    {
        angle.y -= 2 * DirectX::XM_PI;
    }
    UpdateInvincibleTimer(elapsedTime);
    UpdateTransform();
    model->UpdateTransform(transform);
    timer++;
}

void ItemHeel::Render(const RenderContext& rc, ModelShader* shader)
{
    RenderContext myRc = rc;
    myRc.cubicColorData.shaderFlag = CUBIC_DEFAULT;

    shader->Draw(rc, model);
}
