
#ifndef AHA_SERIALIZERARRAY_H
#define AHA_SERIALIZERARRAY_H

#include <stdint.h>

class HASerializerArray
{
public:
    HASerializerArray(const uint8_t size);
    ~HASerializerArray();

    inline uint8_t getItemsNb() const
        { return _itemsNb; }

    inline const char* getItems() const
        { return *_items; }

    bool add(const char* itemP);
    uint16_t calculateSize() const;
    bool serialize(char* output) const;

private:
    uint8_t _size;
    uint8_t _itemsNb;
    const char** _items;
};

#endif
