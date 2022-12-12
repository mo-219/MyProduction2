#pragma once
#ifndef LOAD_H
#define LOAD_H

#include <string>
#include <map>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <DirectXMath.h>

#include "Param.h"
#include "Collision.h"


#include "SerializeFunc.h"


struct LoadData
{
    std::string code;
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT3 rotate;
    DirectX::XMFLOAT3 translate;
    /*std::string model_filename;*/


    std::vector<LoadData> objects;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(code),
            CEREAL_NVP(scale),
            CEREAL_NVP(rotate),
            CEREAL_NVP(translate),
            /*CEREAL_NVP(model_filename),*/
            CEREAL_NVP(objects)
        );
    }
};



class CollisionBehavior
{
public:
    virtual bool collision(Param paramA, Param paramB, DirectX::XMFLOAT3& outB) = 0;
    virtual void hit() = 0;
};

class SphereBehavior : public CollisionBehavior
{
public:
    bool collision(Param paramA, Param paramB, DirectX::XMFLOAT3& outB)override;
    void hit()override;
};
extern SphereBehavior sphereBehavior;

class CylinderBehavior : public CollisionBehavior
{
public:
    bool collision(Param paramA, Param paramB, DirectX::XMFLOAT3& outB)override;
    void hit()override;
};
extern CylinderBehavior cylinderBehavior;

class CubeBehavior : public CollisionBehavior
{
public:
    bool collision(Param paramA, Param paramB, DirectX::XMFLOAT3& outB)override;
    void hit()override;

};
extern CubeBehavior cubeBehavior;


class NoneBehavior : public CollisionBehavior
{
public:
    bool collision(Param paramA, Param paramB, DirectX::XMFLOAT3& outB)override { return false; };
    void hit()override {};

};
extern NoneBehavior noneBehavior;


class RayCastBehavior
{
public:
    virtual bool collision(DirectX::XMFLOAT3& start, DirectX::XMFLOAT3& end, Model* model, HitResult& result);
    virtual void hit();
};
extern RayCastBehavior rayCastBehavior;


class NoneRayCastBehavior : public RayCastBehavior
{
public:
    bool collision(DirectX::XMFLOAT3& start, DirectX::XMFLOAT3& end, Model* model, HitResult& result)override { return false; }
    void hit() {}
};
extern NoneRayCastBehavior noneRayCastBehavior;

struct ObjData
{
    const char* modelName;
    CollisionBehavior* behavior;
    RayCastBehavior* rayCast;

    float radius = 0.5f;
    float width = 1.0f;
    float height = 1.0f;
    float depth = 1.0f;

    DirectX::XMFLOAT3 scale {1,1,1};

};



class LoadObj
{
public:
    LoadObj() {}
    ~LoadObj() {}

    void Load(const char* filename);
    void Create();
    void Create(std::map<std::string, ObjData> Map);

private:
    LoadData loadData = {};
};


extern std::map<std::string, ObjData> stage1_mapData;


#endif /* LOAD_H */