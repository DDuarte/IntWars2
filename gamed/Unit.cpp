#include "stdafx.h"
#include "Server.h"
#include "RafDB.h"
#include "Unit.h"
#include "MovementAnsPacket.h"


bool IsValidModel(char *szModel) {
    return RafDB::GetCharacter(szModel) == true;
}

CIniBin CUnit::GetIniBin()  {
    return RafDB::GetCharacter(szModel);
}

//CUnit
void CUnit::SetName(const char *szName) {
    if(szName) {
        strncpy(this->szName, szName, 32);
    }
}
bool CUnit::SetType(const char *szType) {
    if(szType && IsValidModel((char *)szType)) {
        strncpy(this->szType, szType, 32);
        SetModel(szType);
        return true;
    }
    return false;
}
bool CUnit::SetModel(const char *szModel) {
    if(szModel && IsValidModel((char *)szModel)) {      //check if model exists
        strncpy(this->szModel, szModel, 32);
        return true;
    }
    return false;
}

void CUnit::moveTo(Vector2f pos) {
    MovementAnsPacket::QueueUpdate(this->GetNetworkID(), pos);
}

void CUnit::moveTo(Vector2f pos, Vector2f target, float range) {
    auto n = pos - target;
    n.normalize();
    moveTo(target + n * range);
}

std::string CUnit::GetVar(const char *szSection, const char *szVar) {
    return GetIniBin().GetVar((char *)szSection, (char *)szVar);
}

const char *CUnit::GetModel() {
    return szModel;
}

const char *CUnit::GetType() {
    return szType;
}

const char *CUnit::GetName() {
    return szName;
}

CUnit *CUnit::GetActor(WORD wIndex /*= 0*/) {
    if(actor == NULL && wIndex > 0) {
        actor = new CUnit(wIndex, NULL, NULL, tTeam, x, y);
    }
    return actor;
}

CUnit::CUnit(WORD index, char *szName, char *szType /*= NULL*/, Team tTeam /*= NEUTRAL*/, float x /*= 0*/, float y /*= 0*/) : CUnit() {
    this->index = index;
    this->x = x;
    this->y = y;
    this->tTeam = tTeam;
    SetName(szName);
    SetType(szType);
    SetModel(szType);
}

CUnit::CUnit() : CMovable() {
    ZeroMemory(szName, sizeof(CUnit) - sizeof(CMovable));
    actor = nullptr;
    index = 0;
    tTeam = NEUTRAL;
}

float CUnit::GetY() {
    return GetPos().y;
}

float CUnit::GetX() {
    return GetPos().x;
}

void CObject::SetPos(float fX, float fY, float fZ /*= 0*/) {
    x = fX;
    y = fY;
    z = fZ;
}

DWORD CObject::GetNetworkID() {
    return NWID_BASE | index;
}

CObject::CObject(WORD index, float x, float y) : CObject() {
    this->index = index;
    this->x = x;
    this->y = y;
}

CObject::CObject() {
    index = 0;
    x = 0;
    y = 0;
    z = 0;
}

bool CObject::operator==(const CObject &cmp) {
    return index == cmp.index;
}

double CMovable::GetTimeDelta() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(Time::now() - lastUpdateTime).count();
}

std::vector<Vector2f> CMovable::GetAllWaypoints() {
    return waypoints;
}

void CMovable::Resume() {
    if(bPause) {
        this->lastUpdateTime = Time::now();
        bPause = false;
    }
}

void CMovable::Stop() {
    SetWaypoints(vector<Vector2f>());
}

void CMovable::Pause() {
    if(!bPause) {
        targetRotation = GetRotation();
        SetWaypoints(GetWaypoints());
        bPause = true;
    }
}

void CMovable::SetSpeed(float speed) {
    if(!bPause) {
        SetWaypoints(GetWaypoints());
    }
    mMoveSpeed = speed;
}

float CMovable::GetSpeed() {
    return mMoveSpeed;
}

void CMovable::SetPos(float x, float y, float z) {
    //todo: somehow y and z are swapped here
    this->x = x;
    this->y = y;
    this->z = z;
    auto wps = vector<Vector2f>();
    wps.push_back(Vector2f(x, y));
    SetWaypoints(wps);
}

std::vector<Vector2f> CMovable::GetWaypoints() {
    if(bPause || waypoints.size() <= 1) {
        return waypoints;
    }
    std::vector<Vector2f> newWaypoints;
    auto remainingDistance = GetSpeed() * GetTimeDelta();
    for(int i = 1; i < waypoints.size(); i++) {
        auto current = waypoints[i - 1];
        auto next = waypoints[i];
        auto diff = next - current;
        auto distance = diff.length();
        if(distance > remainingDistance) {
            diff.normalize();
            newWaypoints.push_back(remainingDistance > 0 ? (current + diff * remainingDistance) : current);
        }
        remainingDistance -= distance;
    }
    newWaypoints.push_back(waypoints[waypoints.size() - 1]);
    return newWaypoints;
}

void CMovable::SetWaypoints(std::vector<Vector2f> waypoints) {
    Resume();
    if(waypoints.size() < 1) {
        waypoints.push_back(GetPos()); //make sure there is at least one way point
    }
    initialRotation = GetRotation();
    targetRotation = GetRotation();
    this->lastRotationUpdateTime = Time::now();
    this->lastUpdateTime = Time::now();
    this->waypoints = waypoints;
}

float CMovable::GetRotation() {
    float deltaT = GetRotationTimeDelta();
    if(!bPause) {
        auto remainingDistance = GetSpeed() * GetTimeDelta();
        bool isOnEnd = true;
        for(int i = 1; i < waypoints.size(); i++) {
            auto current = waypoints[i - 1];
            auto next = waypoints[i];
            auto diff = (next - current);
            auto distance = diff.length();
            if(distance > remainingDistance) {
                if(i > 1) {
                    auto v1 = waypoints[i - 1] - waypoints[i - 2];
                    initialRotation = 360 - std::fmod(std::atan2(v1.y, v1.x) / M_PI * 180 + 270, 360);
                }
                auto v2 = waypoints[i] - waypoints[i - 1];
                targetRotation = 360 - std::fmod(std::atan2(v2.y, v2.x) / M_PI * 180 + 270, 360);
                isOnEnd = false;
                break;
            }
            remainingDistance -= distance;
        }
        if(isOnEnd && waypoints.size() > 1) {
            auto v = waypoints[waypoints.size() - 1] - waypoints[waypoints.size() - 2];
            targetRotation = 360 - std::fmod(std::atan2(v.y, v.x) / M_PI * 180 + 270, 360);
            initialRotation = targetRotation;
        }
        deltaT = remainingDistance / GetSpeed();
    }
    auto maxRotation = rotationSpeed * deltaT;
    auto deltaRotation = targetRotation - initialRotation;
    while(deltaRotation < 180) {
        deltaRotation += 360;
    }
    while(deltaRotation > 180) {
        deltaRotation -= 360;
    }
    int sign = deltaRotation < 0 ? -1 : deltaRotation > 0;
    deltaRotation = fmin(fabs(deltaRotation), maxRotation) * sign;
    return initialRotation + deltaRotation;
}

Vector2f CMovable::GetPos() {
    if(bPause) {
        return waypoints.size() > 0 ? waypoints[0] : Vector2f(0, 0);
    }
    auto remainingDistance = GetSpeed() * GetTimeDelta();
    for(int i = 1; i < waypoints.size(); i++) {
        auto current = waypoints[i - 1];
        auto next = waypoints[i];
        auto diff = (next - current);
        auto distance = diff.length();
        if(distance > remainingDistance) {
            diff.normalize();
            return current + diff * remainingDistance;
        }
        remainingDistance -= distance;
    }
    return waypoints.size() > 0 ? waypoints[waypoints.size() - 1] : Vector2f(0, 0);
}

CMovable::CMovable() {
}

CMovable::~CMovable() {
}

void CMovable::RotateTo(Vector2f fTarget) {
    Stop();
    Vector2f v = fTarget - GetPos();
    lastRotationUpdateTime = Time::now();
    targetRotation = 360 - std::fmod(std::atan2(v.y, v.x) / M_PI * 180 + 270, 360);
}

float CMovable::GetDistanceOf(std::vector<Vector2f> points) {
    float total = .0f;
    for(int i = 1; i < points.size(); i++) {
        total += (points[i] - points[i - 1]).length();
    }
    return total;
}

float CMovable::GetRotationSpeed() {
    return rotationSpeed;
}

void CMovable::SetRotationSpeed(float rotationSpeed) {
    this->rotationSpeed = rotationSpeed;
}

double CMovable::GetRotationTimeDelta() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(Time::now() - this->lastRotationUpdateTime).count();
}
