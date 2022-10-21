#pragma once

#include <string>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <DirectXMath.h>

struct GameObjectData
{
	std::string name;
	std::string model_filename;
	std::string texture_filename;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotate;
	DirectX::XMFLOAT3 translate;
	std::vector<GameObjectData> objects;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(name),
			CEREAL_NVP(model_filename),
			CEREAL_NVP(texture_filename),
			CEREAL_NVP(scale),
			CEREAL_NVP(rotate),
			CEREAL_NVP(translate),
			CEREAL_NVP(objects)
		);
	}
};

