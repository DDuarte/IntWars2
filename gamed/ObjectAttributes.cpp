#include "stdafx.h"
#include "ObjectAttributes.h"

unsigned char ObjectAttributeIndex::getMajorMask() {
    return 1 << major;
}

bool ObjectAttributeIndex::operator==(const ObjectAttributeIndex &other) const {
    return (major == other.major
            && minor == other.minor);
}

unsigned int ObjectAttributeIndex::getMinorMask() {
    return 1 << minor;
}
