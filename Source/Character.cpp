#include"Character.h"
#include "Stage.h"

// 行列更新処理
void Character::UpdateTransform()
{

    
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);


    // 回転行列を作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);


    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);


    // ３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;    //左手系


    // 項の順番によって答えは変わる  これだけとは限らない
    // DirectX::XMMATRIX W = S * T1 * R * T2 * T;
    // 3Dゲームの8割はこのままで大丈夫　　やりたい順番に書く

    // 右手系の場合
    // DirectX::XMMATRIX W = T * R * S;    //左手系
    // 掛け算が逆




    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}


// 移動処理
//void Character::Move(float elapsedTime, float vx, float vz, float speed)
void Character::Move(float vx, float vz, float speed)
{
    //speed *= elapsedTime;
    //position.x += vx * speed;
    //position.z += vz * speed;

    // 移動方向ベクトルを設定
    moveVecX = vx;
    moveVecZ = vz;

    // 最大速度設定
    maxMoveSpeed = speed;
}

// 旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;

    DirectX::XMFLOAT3 vec1;
    vec1.x = vx;
    vec1.z = vz;
    float vec = (vec1.x * vec1.x + vec1.z * vec1.z);        //二乗しただけでも0ベクトルの判定できる

    // 進行ベクトルがゼロベクトルの場合は処理する必要なし
    if (vec < 0.00001f)     return;

    vec = sqrtf(vec1.x * vec1.x + vec1.z * vec1.z);         // 判定した後に正規化

    // 進行ベクトルを単位ベクトル化
    vec1.x /= vec;        // 進行ベクトルの単位ベクトル
    vec1.z /= vec;        // 進行ベクトルの単位ベクトル

    // 自身の回転値から前方向を求める
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    vec = sqrtf(frontX * frontX + frontZ * frontZ);
    frontX /= vec;
    frontZ /= vec;


    float dot = (vec1.x * frontX) + (vec1.z * frontZ);    // 内積
    float rot = 1.0f - dot;                               // 補正値
    if (rot > speed) rot = speed;                         // rotが角度っぽくなる    rotは角度じゃないよ！！！！！！！！！！！

    // 左右判定を行うために2つの単位ベクトルの外積を計算する
    float cross = (frontZ * vec1.x) - (frontX * vec1.z);

    // 2Dの外積値が正の場合か負の場合によって左右判定が行える
    // 左右判定を行うことによって左右回転を選択する
    if (cross < 0.0f)
    {
        angle.y -= atanf(rot);
    }
    else
    {
        angle.y += atanf(rot);

    }
}

// ジャンプ処理
void Character::Jump(float speed)
{
    // 上方向の力を設定
    velocity.y += speed;
}

// 速力処理更新
void Character::UpdateVelocity(float elapsedTime)      // 後で色々変えること
{

#if 0
    // 重力処理
    velocity.y += gravity * elapsedFrame;           // 何フレームでなった力

    // 移動処理
    position.y += velocity.y * elapsedTime;

    // 地面判定
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0f;

        if (!IsGround())    // OnLanding関数が何度も呼ばれないように
        {
            OnLanding();
        }
        isGround = true;
    }
    else
    {
        isGround = false;
    }
#endif

    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;

    // 速力更新
    UpdateVerticalVelocity(elapsedFrame);    // 垂直速力更新処理
    UpdateHorizontalVelocity(elapsedFrame);  // 水平速力更新処理

    // 移動更新
    UpdateVerticalMove(elapsedTime);         // 垂直移動更新処理
    UpdateHorizontalMove(elapsedTime);       // 水平移動更新処理

}

// ダメージを与える
bool Character::ApplyDamage(int damage, float invincibleTime)
{
    if (invincibleTimer > 0.0f) return false;           // 名前が似てるので判別しやすいようにthis->invincibleTimerと書いてもよい
    // ダメージが0の場合は健康状態を変更する必要がない
    if (damage <= 0)    return false;

    // 死亡している場合は健康状態を変更しない
    if (health <= 0)    return false;

    // ダメージ処理
    health -= damage;

    // 死亡通知
    if (health <= 0)
    {
        OnDead();
    }
    else
    {
        // ダメージ通知
        OnDamaged();
        invincibleTimer = invincibleTime;
    }

    // 健康状態が変更された場合trueを返す
    return true;
}

void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
        if (invincibleTimer <= 0.0f)    invincibleTimer = 0.0f;
    }
}

// 垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
    // 重力処理
    velocity.y += gravity * elapsedFrame;           // 何フレームでなったか

}

// 水平移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
#if 0
    // 移動処理
    position.y += velocity.y * elapsedTime;

    // 地面判定
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0f;

        if (!IsGround())    // OnLanding関数が何度も呼ばれないように
        {
            OnLanding();
        }
        isGround = true;
    }
    else
    {
        isGround = false;
    }
#endif

    // 垂直方向の移動量
    float my = velocity.y * elapsedTime;

    slopeRate = 0.0f;

    // 落下中
    if (my < 0.0f)
    {
        // レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        // レイキャストによる地面判定
        HitResult hit;

        if (Stage::Instance().RayCast(start, end, hit))
        {
            // 地面に接地している
            position.y = hit.position.y;

            // 傾斜率の計算
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            // 着地した
            if (!isGround)
            {
                OnLanding();
            }

            velocity.y = 0.0f;
            isGround = true;
        }
        else
        {
            // 空中に浮いている
            position.y += my;
            isGround = false;
        }
    }

    // 上昇中
    else if (my > 0.0f)
    {
        position.y += my;
        isGround = false;
    }
}

// 衝撃を与える
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // 速力に力を加える
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;

}


// 水平速力更新処理
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
    // XZ平面の速力を減速する
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // 摩擦力
        float friction = this->friction * elapsedFrame;
        // 足元の材質が変われば摩擦力の大きさも変化する

        // 空中にいるときは摩擦力を減らす
        if (!isGround)   friction *= airControl;

        // 摩擦による横方向の減速処理
        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;

            //length -= friction;
            velocity.x -= vx*friction;
            velocity.z -= vz*friction;
        }
        // 横方向の速力が摩擦力以下になったので速力を無効化
        else
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    // XZ平面の速力を加速する
    if (length <= maxMoveSpeed)
    {
        // 移動ベクトルがゼロベクトルでないなら(スティックの傾きがゼロでないなら)
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            // 加速力
            float acceleration = this->acceleration * elapsedFrame;
            // 足元の材質が変われば加速力の大きさも変化する

            // 空中にいるときは加速力を減らす
            if (!isGround)   acceleration *= airControl;        

            // 移動ベクトルによる加速処理
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            // 最大速度制限
            
            length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vz = velocity.z / length;

                velocity.x = vx * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }

            // 下り坂でガタガタしないようにする
            if (isGround && slopeRate > 0.0f)
            {
                velocity.y -= (length * slopeRate) * elapsedFrame;
            }

        }

    }

    // 移動ベクトルをリセット
    // 最後にmoveVecを0にする
    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

// 水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime)
{
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}
