#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <set>
#include "GameObject.h"
#include "Collision.h"
#include <DirectXMath.h>


// ゲームオブジェクトマネージャー
class ObjectManager
{
private:
    ObjectManager() {}
    ~ObjectManager();

public:
    // 唯一のインスタンス取得
    static ObjectManager& Instance()
    {
        static ObjectManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);
    void UpdateOnlyTransform(float elapsedTime);

    // 初期設定
    void Initialize();

    // 描画処理
    void Render(const RenderContext& rc, ModelShader* shader);

    // ゲームオブジェクト登録
    void Register(GameObject* enemy);

    // ゲームオブジェクト削除
    void Remove(GameObject* enemy);

    // ゲームオブジェクト全削除
    void Clear();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    void DrawDebugGUI();

    // ゲームオブジェクト数取得
    int GetObjectCount() const { return static_cast<int>(objects.size()); }

    // エネミーゲームオブジェクト取得
    GameObject* GetObj(int index) { return objects.at(index); }

    // あたり判定
    void Collision(Param paramB, DirectX::XMFLOAT3& out);

    bool RayCast(Param paramB, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, HitResult& hit);
    bool RayCast(Param paramB, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, HitResult& hit, int name);
    void setNearNum(int a) { nearNum = a; }
    int getNearNum() { return nearNum; }

    int findNear(DirectX::XMFLOAT3 p);

private:
    //// プレイヤーとエネミーの衝突処理
    //void CollisionEnemyVsEnemies();

    std::vector<GameObject*> objects;
    std::set<GameObject*>    removes;


    int nearNum;

public:
    int count = 30;
};


#endif /* OBJ_H */