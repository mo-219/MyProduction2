#include <memory>
#include "SaveShader.h"


SaveShader::~SaveShader()
{
    saveData.objects.clear();
}

void SaveShader::SaveInit(ShaderDataCubic data)
{
    if (data.code < 0) return;
    data.count = count;
    saveData.objects.emplace_back(data);
    count++;
}

void SaveShader::Save(std::string filename)
{
    std::string str = "Shader/Save/" + filename + ".json";
    std::ofstream os(str, std::ios::trunc);
    cereal::JSONOutputArchive archiveFile(os);
    saveData.serialize(archiveFile);

    saveData.objects.clear();
    count = 0;
}

std::vector<ShaderDataCubic> SaveShader::Load(std::string filename)
{
    ShaderDataCubic loadData;

    std::ifstream ifs;
    ifs.open(filename, std::ios::in);
    if (ifs)
    {
        cereal::JSONInputArchive archive(ifs);
        loadData.serialize(archive);

        ifs.close();
    }

    return loadData.objects;
}
