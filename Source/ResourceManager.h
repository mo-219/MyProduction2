#pragma once

#include <memory>
#include <string>
#include <map>

#include "Graphics/ModelResource.h"

// リソースマネージャー
class ResourceManager
{
private:
    ResourceManager() {}
    ~ResourceManager() {}

public:
    // 唯一のインスタンス取得
    static ResourceManager& Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    // モデルリソース読み込み
    std::shared_ptr<ModelResource>LoadModelResource(const char* fileName);

private:
    // HACK メモリリークしたらここ変えるweak_ptrに
    using ModelMap = std::map<std::string, std::shared_ptr<ModelResource>>;
    ModelMap models;
};
