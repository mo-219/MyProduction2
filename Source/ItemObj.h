#pragma once
#include "GameObject.h"
#include "Audio/Audio.h"


class Character;

class ItemHeel : public GameObject
{
public:
    ItemHeel(int i);
    ItemHeel();
    ~ItemHeel();

    void Hit(Character* chara, DirectX::XMFLOAT3 out)override;
    void Update(float elapsedTime)override;
    void Render(const RenderContext& rc, ModelShader* shader)override;

private:
    int timer = 0;
    int heelPoint = 1;

    std::unique_ptr<AudioSource> se;

};
