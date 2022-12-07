#include "ResourceManager.h"
#include "Graphics/Graphics.h"

// ���f�����\�[�X�ǂݍ���
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* fileName)
{
    // ���f������
    ModelMap::iterator ite = models.find(fileName);
    //auto& it = models.find(fileName);

    // ���f��������������
    if (ite != models.end())
    {
        return ite->second;
    }

    // �V�K���f�����\�[�X�쐬���ǂݍ���
    std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
    //auto model = std::make_shared<ModelResource>();

    model->Load(Graphics::Instance().GetDevice(), fileName);

    // �}�b�v�ɓo�^
    //models.emplace(fileName, model);
    models[fileName] = model;

    return model;
}

// weak_ptr�̏ꍇ
//if(ite != models.end())
//{
//  if(!it->second.expired())
//  {
//      // �ǂݍ��ݍς݂̃��f����Ԃ�
//      return ite->second.lock();
//  }
//}



