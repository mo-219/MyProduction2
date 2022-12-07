#include "EnemyManager.h"

void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
    territoryOrigin = origin;
    territoryRange = range;
}

void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}
