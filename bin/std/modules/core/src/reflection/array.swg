#global public
#global namespace Reflection
using Swag

// Returns true if the type 't' is a 'Core.Array' type
func isStructArray(t: typeinfo)->bool
{
    if !t or !isStruct(t) do
        return false

    let tt = cast(const *TypeInfoStruct) t
    return tt.fromGeneric == Core.Array
}

// Returns the generic type of the 'Core.Array' typeinfo.
// If 't' is not a 'Core.Array', returns null
func getStructArrayType(t: typeinfo)->#null typeinfo
{
    if !isStructArray(t) do
        return null

    let tt = cast(const *TypeInfoStruct) t
    return tt.generics[0].pointedType
}

// Used to clear a 'Core.Array' without knowing the type of the element at compile time but knowing it at runtime (valueType)
func clearStructArray(arrayPtr: *void, valueType: typeinfo)
{
    let arr = cast(*Array'u8) arrayPtr

    if isStruct(valueType)
    {
        let valueTypeStruct = cast(const *TypeInfoStruct) valueType
        if valueTypeStruct.opDrop
        {
            var scan = arr.buffer
            for x in arr.count
            {
                valueTypeStruct.opDrop(scan)
                scan += valueTypeStruct.sizeof
            }
        }
    }

    arr.count = 0
}

// Remove an element in a 'Core.Array' without knowing the type of the element at compile time but knowing it at runtime (valueType)
func removeValueStructArray(arrayPtr: *void, valueType: typeinfo, index: u64)
{
    // We will treat the array as a buffer of bytes, just to be able to access the fields
    let arr       = cast(*Array'u8) arrayPtr
    let sizeValue = valueType.sizeof

    var valueTypeStruct: const *TypeInfoStruct
    if isStruct(valueType) do
        valueTypeStruct = cast() valueType

    Debug.assert(index < arr.count)

    // Move all elements at the remove position
    if index != arr.count - 1
    {
        let countMove = arr.count - index - 1
        Memory.copy(arr.buffer + (index * sizeValue), arr.buffer + (index + 1) * sizeValue, countMove * sizeValue)
        if valueTypeStruct and Reflection.hasPostMove(valueTypeStruct)
        {
            for i in index until arr.count - 1 do
                valueTypeStruct.opPostMove(arr.buffer + (i * sizeValue))
        }
    }

    // Drop the last element
    if valueTypeStruct and Reflection.hasDrop(valueTypeStruct) do
        valueTypeStruct.opDrop(arr.buffer + (arr.count - 1) * sizeValue)

    arr.count -= 1
}

// Add an element in a 'Core.Array' without knowing the type of the element at compile time but knowing it at runtime (valueType)
func appendValueStructArray(arrayPtr: *void, valueType: typeinfo)->*void
{
    // We will treat the array as a buffer of bytes, just to be able to access the fields
    let arr       = cast(*Array'u8) arrayPtr
    let sizeValue = valueType.sizeof

    var valueTypeStruct: const *TypeInfoStruct
    if isStruct(valueType) do
        valueTypeStruct = cast() valueType

    // If we need to reallocate the buffer, we will also have to deal with
    // structures specials functions
    if arr.count >= arr.capacity
    {
        let newCapacity = Math.max(arr.count + 1, arr.capacity * 2)
        let newBuffer   = cast([*] u8) Memory.alloc(newCapacity * sizeValue, arr.allocator)
        Memory.clear(newBuffer, newCapacity * sizeValue)
        arr.capacity = newCapacity

        // Move old values to the new buffer
        Memory.copy(newBuffer, arr.buffer, arr.count * sizeValue)
        if hasPostMove(valueType)
        {
            var buf = newBuffer
            for arr.count
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
