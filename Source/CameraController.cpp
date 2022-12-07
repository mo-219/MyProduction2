#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

// �X�V����
void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisRX();
    float ay = gamePad.GetAxisRY();

    // �J�����̉�]���x
    float speed = rollSpeed * elapsedTime;

    // �X�e�B�b�N�̓��͒l�ɍ��킹��X��Y������]
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
    // �J�����̉�]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // ��]�s�񂩂�O�����x�N�g�������o��
    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    // �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
    DirectX::XMFLOAT3 eye;
    //eye = �����_ �{�@���x�N�g���@���@��苗���G
    // �����_�F�@CameraController::target
    // ���x�N�g��: -�O���x�N�g�� = -front;
    // ��苗��: CameraController:: renge;
    //���̂Ƃ�range�̓X�J���[�B�ׂ��Ƃ邶��Ȃ���I�I

    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    // �J�����̎��_�ƒ��ӓ_��ݒ�
    Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

}

void CameraController::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {

            // ��]
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