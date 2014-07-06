#include "stdafx.h"
#include <vector>
#include <iterator>
#include <string>
#include "PacketBuffer.h"
#include <sstream>
//#include <iostream>
#include <iomanip>
#include "Packets.h" // MovementVector
#include "Server.h"
const int INIT_SIZE = 16;


PacketBuffer::PacketBuffer() {
    buffer.reserve(INIT_SIZE);
    pos = 0;
}

PacketBuffer::PacketBuffer(unsigned char *buf, unsigned int len) {
    buffer.assign(buf, buf + len);
    pos = 0;
}

PacketBuffer::~PacketBuffer() {
}

unsigned int PacketBuffer::getPos() {
    return pos;
}

void PacketBuffer::setPos(unsigned int position) {
    pos = position;
}

std::string PacketBuffer::toString() {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    for(auto b : buffer) {
        ss << std::setw(2) << static_cast<unsigned>(b) << " ";
    }
    return ss.str();
}

unsigned char *PacketBuffer::data() {
    return buffer.data();
}

unsigned int PacketBuffer::size() {
    return buffer.size();
}

std::vector<unsigned char>::iterator PacketBuffer::begin() {
    return buffer.begin();
}

std::vector<unsigned char>::iterator PacketBuffer::end() {
    return buffer.end();
}

static unsigned char uint8_t_ignore;
template<typename T = unsigned long long> static void encodeVarint(T value, unsigned char *buf, unsigned char &len = uint8_t_ignore) {
    unsigned char i;
    unsigned char maxLength = (sizeof(T) * 8 + 6) / 7;
    for(i = 0; i < maxLength; i++) {
        buf[i] = value & 0x7f;
        value >>= 7;
        if(!value) { break; }
        buf[i] |= 0x80;
    }
    len = i + 1;
}

template<typename T = uint64_t> static T decodeVarint(unsigned char *data, unsigned char &len = uint8_t_ignore) {
    T result = 0;
    unsigned char maxLength = (sizeof(T) * 8 + 6) / 7;
    unsigned char i;
    for(i = 0; i < maxLength; i++) {
        result |= (data[i] & 0x7full) << (i * 7);
        if(!(data[i] & 0x80)) { break; }
    }
    len = i + 1;
    return result;
}

template<> float PacketBuffer::readEncoded<float>() {
    unsigned char *buf = buffer.data() + pos;
    if(*buf == 0xFF) {
        pos += 1;
        return .0f;
    }
    if(*buf == 0xFE) {
        pos += 5;
        return *(float *) & (++buf);
    }
    pos += 4;
    return *(float *)&buf;
}
template<> unsigned char PacketBuffer::insertEncoded<float>(float value) {
    unsigned char len = 0;
    if(fabs(value) < std::numeric_limits<float>::epsilon()) {
        len += insert<unsigned char>(0xFF);
    } else {
        if(*(unsigned char *)&value == 0xFE) {
            len += insert<unsigned char>(0xFE);
        }
        len += insert<float>(value);
    }
    return len;
}

template<> unsigned long long PacketBuffer::readEncoded<unsigned long long>() {
    unsigned char len = 0;
    unsigned long long result = decodeVarint<unsigned long long>(data() + pos, len);
    pos += len;
    return result;
}

template<> unsigned char PacketBuffer::insertEncoded<unsigned long long>(unsigned long long value) {
    unsigned char len;
    unsigned char encoded[10];
    encodeVarint<unsigned long long>(value, encoded, len);
    for(int i = 0; i < len; i++) {
        insert<unsigned char>(encoded[i]);
    }
    return len;
}

template<> unsigned char PacketBuffer::writeEncoded<unsigned long long>(unsigned long long value) {
    unsigned char len;
    unsigned char encoded[10];
    encodeVarint<unsigned long long>(value, encoded, len);
    for(int i = 0; i < len; i++) {
        write<unsigned char>(encoded[i]);
    }
    return len;
}

template<> unsigned char PacketBuffer::writeEncoded<float>(float value) {
    unsigned char len = 0;
    if(fabs(value) < std::numeric_limits<float>::epsilon()) {
        len += write<unsigned char>(0xFF);
    } else {
        if(*reinterpret_cast<unsigned char *>(&value) == 0xFE) {
            len += write<unsigned char>(0xFE);
        }
        len += write<float>(value);
    }
    return len;
}

static inline void SetBitmaskValue(byte mask[], int pos, bool val) {
    if(pos < 0) {
        return;
    }
    if(val) {
        mask[pos / 8] |= 1 << (pos % 8);
    } else {
        mask[pos / 8] &= ~(1 << (pos % 8));
    }
}

static inline bool GetBitmaskValue(byte mask[], int pos) {
    return pos >= 0 && ((1 << (pos % 8)) & mask[pos / 8]) != 0;
}

/* todo: insertEncoded waypoints */
/* todo: use different movementvector size, short for x and y seems too small. value can be between -2^17 and +2^17 */
/* todo: use map size of current map, currently using summoners rift size */
template<> unsigned char PacketBuffer::writeEncoded<std::vector<Vector2f>>(std::vector<Vector2f> waypoints) {
    int startPos = pos;
    int coordCount = 2 * waypoints.size();
    unsigned int maskSize = (coordCount + 5) / 8; //mask size
    buffer.reserve(pos + maskSize + coordCount * sizeof(short)); //reserve max total size
    for(unsigned int i = 0; i < maskSize; i++) {
        write<unsigned char>(0);
    }
    Vector2f lastCoord;
    for(int i = 0; i < waypoints.size(); i++) {
        auto mapSize = (*server).GetMap().GetSize();
        Vector2i curVector((waypoints[i].x - mapSize.x) / 2, (waypoints[i].y - mapSize.y) / 2);
        Vector2i relative(curVector.x - lastCoord.x, curVector.y - lastCoord.y);
        std::pair<bool, bool> isAbsolute(
            i == 0 || relative.x < CHAR_MIN || relative.x > CHAR_MAX,
            i == 0 || relative.y < CHAR_MIN || relative.y > CHAR_MAX);
        SetBitmaskValue(data() + startPos, 2 * i - 2, !isAbsolute.first);
        if(isAbsolute.first) {
            write<short>(curVector.x);
        } else {
            write<char>(relative.x);
        }
        SetBitmaskValue(data() + startPos, 2 * i - 1, !isAbsolute.second);
        if(isAbsolute.second) {
            write<short>(curVector.y);
        } else {
            write<char>(relative.y);
        }
        lastCoord = curVector;
    }
    return pos - startPos;
}

template<> std::vector<Vector2f> PacketBuffer::readEncoded<std::vector<Vector2f>>(unsigned int coordCount) {
    auto mapSize = (*server).GetMap().GetSize();
    unsigned int startPos = pos;
    unsigned char *mask = data() + startPos;
    pos += (coordCount + 5) / 8 + coordCount % 2;
    int vectorCount = coordCount / 2;
    std::vector<Vector2f> vMoves;
    vMoves.reserve(vectorCount);
    Vector2f lastCoord(.0f, .0f);
    for(int i = 0; i < vectorCount; i++) {
        if(GetBitmaskValue(mask, 2 * i - 2)) {
            lastCoord.x += read<char>();
        } else {
            lastCoord.x = read<short>();
        }
        if(GetBitmaskValue(mask, 2 * i - 1)) {
            lastCoord.y += read<char>();
        } else {
            lastCoord.y = read<short>();
        }
        vMoves.push_back(Vector2f(2.0f * lastCoord.x + mapSize.x, 2.0f * lastCoord.y + mapSize.y));
    }
    return vMoves;
}

void PacketBuffer::clear() {
    buffer.clear();
    pos = 0;
}
