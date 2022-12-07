#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

// 更新処理
void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();

    // カメラの回転速度
    float speed = rollSpeed * elapsedTime;

    // スティックの入力値に合わせてX軸Y軸を回転
    angle.x += ay * speed;
    angle.y += ax * speed;
    if (angle.x > maxAngleX)  angle.x = maxAngleX;
    if (angle.x < minAngleX)  angle.x = minAngleX;

    if (angle.y > DirectX::XM_PI)  angle.y -= DirectX::XM_2PI;
    if (angle.y < DirectX::XM_PI)  angle.y += DirectX::XM_2PI;


    UpdateOnlyTransform(elapsedTime);

}

void CameraController::UpdateOnlyTransform(float elapsedTime)
{
    // カメラの回転値を回転行列に変換
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // 回転行列から前方向ベクトルを取り出す
    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    // 中視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
    DirectX::XMFLOAT3 eye;
    //eye = 注視点 ＋　後ろベクトル　＊　一定距離；
    // 注視点：　CameraController::target
    // 後ろベクトル: -前方ベクトル = -front;
    // 一定距離: CameraController:: renge;
    //このときrangeはスカラー。べくとるじゃないよ！！

    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    // カメラの視点と注意点を設定
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

}

void CameraController::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {

            // 回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);

        }
    }
    ImGui::End();
}