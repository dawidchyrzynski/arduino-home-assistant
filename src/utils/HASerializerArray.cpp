
#include <Arduino.h>

#include "HASerializerArray.h"

HASerializerArray::HASerializerArray(const uint8_t size) :
    _size(size),
    _itemsNb(0),
    _items(static_cast<char*>(malloc(size * sizeof(const char*))))
{

}

HASerializerArray::~HASerializerArray()
{
    if (_items) {
        delete _items;
    }
}

bool HASerializerArray::add(const char* itemP)
{
    if (_itemsNb >= _size) {
        return false;
    }

    _items[_itemsNb++] = itemP;
    return true;
}
