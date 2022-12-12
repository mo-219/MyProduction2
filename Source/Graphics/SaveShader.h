#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>

#include <DirectXMath.h>

#include "SerializeFunc.h"



enum OwnerCode
{
    Error = -1,
    Defalt,
    Player = 1,
    Enemy,
    Object,

};


// ���f�[�^
struct ShaderDataCubic
{
    OwnerCode         code;
    int               count;
    int				  shaderFlag;		// CUBIC_DEFAULT: default	CUBIC_CUSTOMIZE: �X�ŐF�ς�
    DirectX::XMFLOAT4 rightVec;							// �E�x�N�g��	w: dummy
    DirectX::XMFLOAT4 topVec;							// ��x�N�g��	w: dummy
    DirectX::XMFLOAT4 frontVec;							// �O�x�N�g��	w: dummy

    DirectX::XMFLOAT4 colorTop1;
    DirectX::XMFLOAT4 colorBottom1;
    DirectX::XMFLOAT4 colorRight1;
    DirectX::XMFLOAT4 colorLeft1;
    DirectX::XMFLOAT4 colorBack1;
    DirectX::XMFLOAT4 colorFront1;

    DirectX::XMFLOAT4 colorTop2;
    DirectX::XMFLOAT4 colorBottom2;
    DirectX::XMFLOAT4 colorRight2;
    DirectX::XMFLOAT4 colorLeft2;
    DirectX::XMFLOAT4 colorBack2;
    DirectX::XMFLOAT4 colorFront2;

    DirectX::XMFLOAT4 colorAlpha;	// a:�@�ق�ƂɎg���A���t�@�l		rgb: dummy



    std::vector<ShaderDataCubic> objects;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(code),
            CEREAL_NVP(count),

            CEREAL_NVP(shaderFlag),

            CEREAL_NVP(rightVec),
            CEREAL_NVP(topVec),
            CEREAL_NVP(frontVec),

            CEREAL_NVP(colorTop1),
            CEREAL_NVP(colorBottom1),
            CEREAL_NVP(colorRight1),
            CEREAL_NVP(colorLeft1),
            CEREAL_NVP(colorBack1),
            CEREAL_NVP(colorFront1),

            CEREAL_NVP(colorTop2),
            CEREAL_NVP(colorBottom2),
            CEREAL_NVP(colorRight2),
            CEREAL_NVP(colorLeft2),
            CEREAL_NVP(colorBack2),
            CEREAL_NVP(colorFront2),

            CEREAL_NVP(colorAlpha),
            CEREAL_NVP(objects)
        );
    }
};


class SaveShader
{
public:
    SaveShader() {}
    ~SaveShader();

    void SaveInit(ShaderDataCubic data);
    void Save(std::string filename);
    
    std::vector<ShaderDataCubic> Load(std::string filename);

    int count = 0;
    ShaderDataCubic saveData = {};

};
