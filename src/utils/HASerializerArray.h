
#ifndef AHA_SERIALIZERARRAY_H
#define AHA_SERIALIZERARRAY_H

#include <stdint.h>

/**
 * HASerializerArray represents array of items that can be used as a HASerializer property.
 * Each item needs to point to the flash memory string.
 */
class HASerializerArray
{
public:
    /**
     * Constructs HASerializerArray with the static size (number of elements).
     * The array is allocated dynamically in the memory based on the given size.
     *
     * @param size The desired number of elements that will be stored in the array.
     */
    HASerializerArray(const uint8_t size);
    ~HASerializerArray();

    /**
     * Returns the number of elements that were added to the array.
     * It can be lower than size of the array.
     */
    inline uint8_t getItemsNb() const
        { return _itemsNb; }

    /**
     * Returns pointer to the array.
     */
    inline const char* getItems() const
        { return *_items; }

    /**
     * Adds a new element to the array.
     * The item needs to point to the flash memory that stores the string.
     * 
     * @param itemP Item to add (flash string).
     * @returns Returns `true` if item has been added to the array successfully.
     */
    bool add(const char* itemP);

    /**
     * Calculates the size of the serialized array (JSON representation).
     */
    uint16_t calculateSize() const;

    /**
     * Serializes array as JSON to the given output.
     */
    bool serialize(char* output) const;

private:
    /// The maximum size of the array.
    uint8_t _size;

    /// The number of items that were added to the array.
    uint8_t _itemsNb;

    /// Pointer to the array elements.
    const char** _items;
};

#endif
