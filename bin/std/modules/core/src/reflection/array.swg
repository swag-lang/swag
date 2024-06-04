#global public
#global namespace Reflection
using Swag

// Used to clear a 'Core.Array' without knowing the type of the element at compile time but knowing it at runtime (valueType)
func clearArray(addr: *void, valueType: typeinfo)
{
    let arr = cast(*Array'u8) addr

    if isStruct(valueType)
    {
        let valueTypeStruct = cast(const *TypeInfoStruct) valueType
        if valueTypeStruct.opDrop
        {
            var scan = arr.buffer
            loop x: arr.count
            {
                valueTypeStruct.opDrop(scan)
                scan += valueTypeStruct.sizeof
            }
        }
    }

    arr.count = 0
}

// Used to add an element in a 'Core.Array' without knowing the type of the element at compile time but knowing it at runtime (valueType)
func appendValueArray(addr: *void, valueType: typeinfo)->*void
{
    // We will treat the array as a buffer of bytes, just to be able to access the fields
    let arr       = cast(*Array'u8) addr
    let sizeValue = valueType.sizeof

    var valueTypeStruct: const *TypeInfoStruct
    if isStruct(valueType) do
        valueTypeStruct = acast valueType

    // If we need to reallocate the buffer, we will also have to deal with
    // structures specials functions
    if arr.count >= arr.capacity
    {
        let newCapacity = Math.max(arr.count + 1, arr.capacity * 2)
        let newBuffer   = cast(^u8) Memory.alloc(newCapacity * sizeValue, arr.allocator)
        Memory.clear(newBuffer, newCapacity * sizeValue)
        arr.capacity = newCapacity

        // Move old values to the new buffer
        Memory.copy(newBuffer, arr.buffer, arr.count * sizeValue)
        if hasPostMove(valueType)
        {
            var buf = newBuffer
            loop arr.count
            {
                valueTypeStruct.opPostMove(buf)
                buf += sizeValue
            }
        }

        Memory.free(arr.buffer, arr.count * sizeValue)
        arr.buffer = newBuffer
    }

    let result = arr.buffer + (arr.count * valueType.sizeof)
    arr.count += 1
    if valueTypeStruct do
        valueTypeStruct.opInit(result)

    return result
}
