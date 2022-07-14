#pragma once

// �V�[��
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    virtual void Initialize() = 0;                     // ������
    virtual void Finalize() = 0;                       // �I����
    virtual void Update(float elapsedTime) = 0;        // �X�V����
    virtual void Render() = 0;                         // �`�揈��

    bool IsReady() const { return ready; }             // �����������Ă��邩
    void SetReady() { ready = true; }                  // ���������ݒ�

private:
    bool ready = false;
};
