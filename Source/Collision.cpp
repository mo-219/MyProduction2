#include "Collision.h"
#include "Graphics/Model.h"

bool Collision::InstersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB)
{
    float lengthSq;
    // B��A�̒P�ʃx�N�g���Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec       = DirectX::XMVectorSubtract(PositionB, PositionA);

    DirectX::XMVECTOR LengthSq  = DirectX::XMVector3Length(Vec);
    // ���̉񓚁�
    // DirectX::XMVECTOR LengthSq  = DirectX::XMVectorSqrt(DirectX::XMVector3Dot(vec, vec));

    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // ��������
    float range = radiusA + radiusB;
    if (lengthSq > range)    return false;


    // AB�������o��
    //DirectX::XMVECTOR Range = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3{ range, range, range });
    //DirectX::XMVECTOR Sub = DirectX::XMVectorSubtract(vec, Range);

    Vec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), range);
    PositionB = DirectX::XMVectorAdd(PositionA, Vec);
    DirectX::XMStoreFloat3(&outPositionB, PositionB);

    // ���̉񓚁�
    // DirectX::XMVECTOR OutPositionB = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), (range - lengthSq));
    // DirectX::XMStoreFloat3(&outPositionB, OutPositionB);
    return true;
}

bool Collision::InstersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB)
{
    float topA      = positionA.y + heightA;
    float topB      = positionB.y + heightB;
    float bottomA   = positionA.y;
    float bottomB   = positionB.y;

    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (bottomA > topB)     return false;

    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (topA < bottomB)     return false;

    // XZ���ʂł͈̔̓`�F�b�N

    float vecX = (positionB.x - positionA.x);
    float vecZ = (positionB.z - positionA.z);

    float lengthAB = sqrtf(vecX * vecX + vecZ * vecZ);
    float Range = radiusA + radiusB;

    if (Range < lengthAB)   return false;

    // A��B�������o��
    outPositionB.x = positionA.x + ( vecX / lengthAB * Range );
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + ( vecZ / lengthAB * Range );


    return true;

}

// ���Ɖ~���̌�������
bool Collision::InstersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition,
                                           float sphereRadius,
                                           const DirectX::XMFLOAT3& cylinderPosition,
                                           float cylinderRadius,
                                           float cylinderHeight,
                                           DirectX::XMFLOAT3& outCylinderPosition)
{
    float sqhereTop = spherePosition.y + sphereRadius*2;
    float sqhereBottom = spherePosition.y;

    float cylinderTop = cylinderPosition.y + cylinderHeight;
    float cylinderBottom = cylinderPosition.y;

    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (sqhereBottom > cylinderTop)     return false;

    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (sqhereTop < cylinderBottom)     return false;


        // XZ���ʂł͈̔̓`�F�b�N

    float vecX = (cylinderPosition.x - spherePosition.x);
    float vecZ = (cylinderPosition.z - spherePosition.z);

    float lengthAB = sqrtf(vecX * vecX + vecZ * vecZ);
    float Range = sphereRadius + cylinderRadius;

    if (Range < lengthAB)   return false;

    // A��B�������o��
    outCylinderPosition.x = spherePosition.x + ( vecX / lengthAB * Range );
    outCylinderPosition.y = cylinderPosition.y;
    outCylinderPosition.z = spherePosition.z + ( vecZ / lengthAB * Range );


    return true;


}

bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
    DirectX::XMVECTOR WorldStart        = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd          = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec       = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength    = DirectX::XMVector3Length(WorldRayVec);

    // ���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();

    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        // ���b�V���m�[�h�擾
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        DirectX::XMVECTOR NodePos = DirectX::XMLoadFloat3(&node.translate);
        DirectX::XMVECTOR NodeVec = DirectX::XMVectorSubtract(NodePos, WorldStart);
        DirectX::XMVECTOR NodeLength = DirectX::XMVector3Length(NodeVec);

       

        // ���C�����[���h���W���烍�[�J����Ԃ֕ϊ�
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
 
        DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        // ���C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        // �O�p�`(��)�Ƃ̌�������
        const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;

        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            // �P���b�V���̃|���S���m�F
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                // �O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index+0));
                const ModelResource::Vertex& b = vertices.at(indices.at(index+1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index+2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                // �O�p�`�̎O�Ӄx�N�g�����Z�o
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                // �O�p�`�̖@���x�N�g�����Z�o
                DirectX::XMVECTOR N         = DirectX::XMVector3Cross(AB, BC);
                N = DirectX::XMVector3Normalize(N);

                // ���ς̌��ʂ��{�Ȃ�Η�����
                DirectX::XMVECTOR Dot       = DirectX::XMVector3Dot(V,N);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);

                if (dot >= 0.0f) continue;                  // �������Ȃ玟��

                // ���C�ƕ��ʂ̌�_���Z�o
                DirectX::XMVECTOR SA        = DirectX::XMVectorSubtract(A, S);
                DirectX::XMVECTOR X         = DirectX::XMVectorDivide(DirectX::XMVector3Dot(SA, N), Dot);
                // X = (SA�EN) / (V�EN)

                float x;
                DirectX::XMStoreFloat(&x, X);
                if (x < .0f || x > neart)   continue;      // ��_�܂ł̋��������܂łɌv�Z����
                                                            // �ŋߋ������傫���Ƃ��̓X�L�b�v
                DirectX::XMVECTOR P         = DirectX::XMVectorAdd(S, DirectX::XMVectorMultiply(V, X));

                // ��_���O�p�`�̓����ɂ��邩����
                // �P��
                DirectX::XMVECTOR PA        = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1    = DirectX::XMVector3Cross(PA, AB);
                DirectX::XMVECTOR Dot1      = DirectX::XMVector3Dot(N, Cross1);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);

                if (dot1 <= 0)   continue;

                // �Q��
                DirectX::XMVECTOR PB        = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2    = DirectX::XMVector3Cross(PB, BC);
                DirectX::XMVECTOR Dot2      = DirectX::XMVector3Dot(N, Cross2);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);

                if (dot2 <= 0)   continue;

                // �R��
                DirectX::XMVECTOR PC        = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3    = DirectX::XMVector3Cross(PC, CA);
                DirectX::XMVECTOR Dot3      = DirectX::XMVector3Dot(N, Cross3);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);

                if (dot3 <= 0)   continue;

                // �ŋߋ������X�V
                neart = x;

                // ��_�Ɩ@�����X�V
                HitPosition = P;
                HitNormal = N;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            // �������ĂȂ�������-1�A����������O�ȏオ�i�[����Ă���
            // ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR WorldPosition =
                DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);

            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            // �q�b�g���ۑ�
            // ���̃��b�V���Ɣ�ׂčŋߋ������r(���b�V������łȂ��\��������)

            if (result.distance > distance)
            {
                DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
                WorldNormal = DirectX::XMVector3Normalize(WorldNormal);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));

                hit = true;
            }
        }
    }
    
    return hit;
}


bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    float judgeRadius,
    HitResult& result)
{
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    // ���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();

    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        // ���b�V���m�[�h�擾
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        DirectX::XMVECTOR NodePos = DirectX::XMLoadFloat3(&node.translate);
        DirectX::XMVECTOR NodeVec = DirectX::XMVectorSubtract(NodePos, WorldStart);
        DirectX::XMVECTOR NodeLength = DirectX::XMVector3Length(NodeVec);



        // ���C�����[���h���W���烍�[�J����Ԃ֕ϊ�
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        // ���C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        // �O�p�`(��)�Ƃ̌�������
        const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;



        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            // �P���b�V���̃|���S���m�F
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                // �O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index + 0));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                DirectX::XMVECTOR WorldPosition =
                    DirectX::XMVector3TransformCoord(A, WorldTransform);
                DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
                DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
                float distance;
                DirectX::XMStoreFloat(&distance, WorldCrossLength);

                if (distance > judgeRadius)
                {
                    continue;
                }

                // �O�p�`�̎O�Ӄx�N�g�����Z�o
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                // �O�p�`�̖@���x�N�g�����Z�o
                DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);
                N = DirectX::XMVector3Normalize(N);

                // ���ς̌��ʂ��{�Ȃ�Η�����
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);

                if (dot >= 0.0f) continue;                  // �������Ȃ玟��

                // ���C�ƕ��ʂ̌�_���Z�o
                DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
                DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(SA, N), Dot);
                // X = (SA�EN) / (V�EN)

                float x;
                DirectX::XMStoreFloat(&x, X);
                if (x < .0f || x > neart)   continue;      // ��_�܂ł̋��������܂łɌv�Z����
                                                            // �ŋߋ������傫���Ƃ��̓X�L�b�v
                DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorMultiply(V, X));

                // ��_���O�p�`�̓����ɂ��邩����
                // �P��
                DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(N, Cross1);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);

                if (dot1 <= 0)   continue;

                // �Q��
                DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
                DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(N, Cross2);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);

                if (dot2 <= 0)   continue;

                // �R��
                DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
                DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(N, Cross3);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);

                if (dot3 <= 0)   continue;

                // �ŋߋ������X�V
                neart = x;

                // ��_�Ɩ@�����X�V
                HitPosition = P;
                HitNormal = N;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            // �������ĂȂ�������-1�A����������O�ȏオ�i�[����Ă���
            // ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR WorldPosition =
                DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);

            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            // �q�b�g���ۑ�
            // ���̃��b�V���Ɣ�ׂčŋߋ������r(���b�V������łȂ��\��������)

            if (result.distance > distance)
            {
                DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
                WorldNormal = DirectX::XMVector3Normalize(WorldNormal);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));

                hit = true;
            }
        }
    }

    return hit;
}

bool Collision::InstersectCubeVsCylinder(const DirectX::XMFLOAT3& cubePosition, float width, float height, float depth, const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{
    float cubeTop = cubePosition.y + height;
    float cubeBottom = cubePosition.y;

    float cylinderTop = cylinderPosition.y + cylinderHeight;
    float cylinderBottom = cylinderPosition.y;

    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (cubeBottom > cylinderTop)     return false;

    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (cubeTop < cylinderBottom)     return false;

    float cubeLeft = cubePosition.x - width / 2;
    float cubeRight = cubePosition.x + width / 2;

    float cubeBack = cubePosition.z + depth / 2;
    float cubeFront = cubePosition.z - depth / 2;

    // XZ���ʂł͈̔̓`�F�b�N
    DirectX::XMFLOAT3 NearestPoint = cylinderPosition;  // �����Ƌ��̍ŋߓ_

    if (NearestPoint.x <= cubeLeft)  NearestPoint.x = cubeLeft;
    else if (NearestPoint.x >= cubeRight)  NearestPoint.x = cubeRight;
    else NearestPoint.x = NearestPoint.x;                               // �ω������Ȃ�

    if (NearestPoint.z <= cubeFront)  NearestPoint.z = cubeFront;
    else if (NearestPoint.z >= cubeBack)  NearestPoint.z = cubeBack;

    if (NearestPoint.y <= cubeBottom)  NearestPoint.y = cubeBottom;
    else if (NearestPoint.y >= cubeTop)  NearestPoint.y = cubeTop;

    DirectX::XMFLOAT3 vec = { cylinderPosition.x - NearestPoint.x, cylinderPosition.y - NearestPoint.y, cylinderPosition.z - NearestPoint.z };
    float dis = sqrtf(vec.x * vec.x +
        vec.y * vec.y +
        vec.z * vec.z);

    if (cylinderRadius < dis)    return false;


    float len = cylinderRadius - dis;
    if (len < 0)    len *= (-1);

    outCylinderPosition.x = cylinderPosition.x + (vec.x / dis * len);
    outCylinderPosition.y = cylinderPosition.y;
    outCylinderPosition.z = cylinderPosition.z + (vec.z / dis * len);


    return true;
}
bool Collision::InstersectCubeVsSphere(const DirectX::XMFLOAT3& cubePosition, float width, float height, float depth, const DirectX::XMFLOAT3& spherePosition, float sphereRadius,  DirectX::XMFLOAT3& outCylinderPosition)
{
    return false;
}

