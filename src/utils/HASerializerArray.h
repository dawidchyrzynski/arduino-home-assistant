
#ifndef AHA_SERIALIZERARRAY_H
#define AHA_SERIALIZERARRAY_H

#include <stdint.h>

class HASerializerArray
{
public:
    HASerializerArray(const uint8_t size);
    virtual ~HASerializerArray();

    inline const uint8_t getItemsNb() const
        { return _itemsNb; }

    inline const char* getItems() const
        { return _items; }

    bool add(const char* itemP);
    uint16_t calculateSize() const;
    bool serialize(char* destination) const;

private:
    uint8_t _size;
    uint8_t _itemsNb;
    char* _items;
};

#endif
