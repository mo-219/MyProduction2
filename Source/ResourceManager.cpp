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



// 授業課題ではテクスチャ情報しかもっていない
// スキンドメッシュみたいにモーションをもってるやつはここの動きをしてほしいのでモーションはモデルリソースに向いてない
// ボーン群、アニメーションとかはいいかも
// なんだったら使いまわせるかは考えながら組み立てていくべき
// 
// 使いまわせないものは保持しない、使いまわせるものは保持しよう
// ComPtrはDirectX独自のスマートポインタ


// unique_ptr   他で使いまわせない、データの貸し借りが出来ない  　 安全性はこっち
// shared_ptr   共有が出来る。参照されなくなったら破棄される       使い勝手はこっち
// weak_ptr     shaderd_ptrでデッドループに入るやつを守れる       苦肉の策で作ったポインタ　使わないほうが良い