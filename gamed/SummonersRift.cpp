#include "stdafx.h"
#include "SummonersRift.h"

SummonersRift::SummonersRift() {
    meshFileName = "LEVELS\\Map1\\Scene\\AIPath.sco";
    navMeshFileName = "LEVELS\\Map1\\AIPath.aimesh";
}


SummonersRift::~SummonersRift() {
}

Vector2i SummonersRift::GetSize() {
    return Vector2i(13982 / 2, 14446 / 2);
}