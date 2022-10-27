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



// ���Ɖۑ�ł̓e�N�X�`����񂵂������Ă��Ȃ�
// �X�L���h���b�V���݂����Ƀ��[�V�����������Ă��͂����̓��������Ăق����̂Ń��[�V�����̓��f�����\�[�X�Ɍ����ĂȂ�
// �{�[���Q�A�A�j���[�V�����Ƃ��͂�������
// �Ȃ񂾂�����g���܂킹�邩�͍l���Ȃ���g�ݗ��ĂĂ����ׂ�
// 
// �g���܂킹�Ȃ����͕̂ێ����Ȃ��A�g���܂킹����͕̂ێ����悤
// ComPtr��DirectX�Ǝ��̃X�}�[�g�|�C���^


// unique_ptr   ���Ŏg���܂킹�Ȃ��A�f�[�^�݂̑��؂肪�o���Ȃ�  �@ ���S���͂�����
// shared_ptr   ���L���o����B�Q�Ƃ���Ȃ��Ȃ�����j�������       �g������͂�����
// weak_ptr     shaderd_ptr�Ńf�b�h���[�v�ɓ���������       ����̍�ō�����|�C���^�@�g��Ȃ��ق����ǂ�