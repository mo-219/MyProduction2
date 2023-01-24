#include "EnemyManager.h"

void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
    territoryOrigin = origin;
    territoryRange = range;
}

bool Enemy::OnMessage(const Telegram& msg)
{
    return false;
}

void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}
