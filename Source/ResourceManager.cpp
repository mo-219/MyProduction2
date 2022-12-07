#include "ResourceManager.h"
#include "Graphics/Graphics.h"

// モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* fileName)
{
    // モデル検索
    ModelMap::iterator ite = models.find(fileName);
    //auto& it = models.find(fileName);

    // モデルが見つかったら
    if (ite != models.end())
    {
        return ite->second;
    }

    // 新規モデルリソース作成＆読み込み
    std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
    //auto model = std::make_shared<ModelResource>();

    model->Load(Graphics::Instance().GetDevice(), fileName);

    // マップに登録
    //models.emplace(fileName, model);
    models[fileName] = model;

    return model;
}

// weak_ptrの場合
//if(ite != models.end())
//{
//  if(!it->second.expired())
//  {
//      // 読み込み済みのモデルを返す
//      return ite->second.lock();
//  }
//}



