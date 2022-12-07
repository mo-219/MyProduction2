#include "StageManager.h"

void Stage::Destroy()
{
    StageManager::Instance().Remove(this);
}
