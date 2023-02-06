#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <set>
#include "GameObject.h"
#include "Collision.h"
#include <DirectXMath.h>


// �Q�[���I�u�W�F�N�g�}�l�[�W���[
class ObjectManager
{
private:
    ObjectManager() {}
    ~ObjectManager();

public:
    // �B��̃C���X�^���X�擾
    static ObjectManager& Instance()
    {
        static ObjectManager instance;
        return instance;
    }

    // �X�V����
    void Update(float elapsedTime);
    void UpdateOnlyTransform(float elapsedTime);

    // �����ݒ�
    void Initialize();

    // �`�揈��
    void Render(const RenderContext& rc, ModelShader* shader);

    // �Q�[���I�u�W�F�N�g�o�^
    void Register(GameObject* enemy);

    // �Q�[���I�u�W�F�N�g�폜
    void Remove(GameObject* enemy);

    // �Q�[���I�u�W�F�N�g�S�폜
    void Clear();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    void DrawDebugGUI();

    // �Q�[���I�u�W�F�N�g���擾
    int GetObjectCount() const { return static_cast<int>(objects.size()); }

    // �G�l�~�[�Q�[���I�u�W�F�N�g�擾
    GameObject* GetObj(int index) { return objects.at(index); }

    // �����蔻��
    void Collision(Param paramB, DirectX::XMFLOAT3& out);

    bool RayCast(Param paramB, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, HitResult& hit);
    bool RayCast(Param paramB, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, HitResult& hit, int name);
    void setNearNum(int a) { nearNum = a; }
    int getNearNum() { return nearNum; }

    int findNear(DirectX::XMFLOAT3 p);

private:
    //// �v���C���[�ƃG�l�~�[�̏Փˏ���
    //void CollisionEnemyVsEnemies();

    std::vector<GameObject*> objects;
    std::set<GameObject*>    removes;


    int nearNum;

public:
    int count = 30;
};


#endif /* OBJ_H */