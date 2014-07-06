#pragma once

//Todo: safety size checking

class PacketBuffer {
    public:
        PacketBuffer();
        PacketBuffer(unsigned char *, unsigned int);
        ~PacketBuffer();

        unsigned int getPos();
        void setPos(unsigned int position);

        void clear();

        template<typename T = unsigned char> T read();
        template<typename T = unsigned char> T readAt(unsigned int);
        template<typename T = unsigned char> T readAt(std::vector<unsigned char>::iterator);

        template<typename T = unsigned char> unsigned char insert(T);
        template<typename T = unsigned char> unsigned char insertAt(T, unsigned int);
        template<typename T = unsigned char> unsigned char insertAt(T, std::vector<unsigned char>::iterator);

        template<typename T = unsigned char> unsigned char write(T);
        template<typename T = unsigned char> unsigned char writeAt(T, unsigned int);
        template<typename T = unsigned char> unsigned char writeAt(T, std::vector<unsigned char>::iterator);

        template<typename T> T readEncoded();
        template<typename T> T readEncoded(unsigned int);
        template<typename T> unsigned char insertEncoded(T);
        template<typename T> unsigned char writeEncoded(T);

        //todo: check if this is enough to iterate in c++11
        std::vector<unsigned char>::iterator begin();
        std::vector<unsigned char>::iterator end();

        unsigned char *data();
        unsigned int size();

        std::string toString();
    private:
        unsigned int pos;
        std::vector<unsigned char> buffer;
};



template<typename T> unsigned char PacketBuffer::writeAt(T var, std::vector<unsigned char>::iterator position) {
    return writeAt(var, std::distance(buffer.begin(), position));
}

template<typename T> unsigned char PacketBuffer::writeAt(T var, unsigned int position) {
    unsigned char *src = reinterpret_cast<unsigned char *>(&var);
    unsigned char sizeT = sizeof(T);
    for(unsigned int i = size(); i < position; i++) {
        insertAt(0, buffer.end());
    }
    for(int i = 0; i < sizeT; i++) {
        if(position < size()) {
            buffer[position++] = *src++;
        } else {
            insertAt(*src++, position++);
        }
    }
    return sizeT;
}

template<typename T> unsigned char PacketBuffer::write(T var) {
    unsigned char sizeT = writeAt<T>(var, pos);
    pos += sizeT;
    return sizeT;
}

template<typename T> unsigned char PacketBuffer::insert(T var) {
    unsigned char sizeT = insertAt<T>(var, pos);
    pos += sizeT;
    return sizeT;
}

template<typename T> unsigned char PacketBuffer::insertAt(T var, unsigned int position) {
    unsigned char *src = reinterpret_cast<unsigned char *>(&var);
    unsigned char sizeT = sizeof(T);
    buffer.insert(buffer.begin() + position, src, src + sizeT);
    return sizeT;
}

template<typename T> unsigned char PacketBuffer::insertAt(T var, std::vector<unsigned char>::iterator position) {
    return insertAt<T>(var, std::distance(buffer.begin(), position));
}

template<typename T> T PacketBuffer::read() {
    T var = *reinterpret_cast<T *>(buffer.data() + getPos());
    pos += sizeof(T);
    return var;
}

template<typename T> T PacketBuffer::readAt(unsigned int position) {
    return *reinterpret_cast<T *>(buffer.data() + position);
}

template<typename T> T PacketBuffer::readAt(std::vector<unsigned char>::iterator position) {
    return *reinterpret_cast<T *>(buffer.data() + std::distance(buffer.begin(), position));
}






