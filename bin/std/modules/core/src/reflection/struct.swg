#global public
#global namespace Reflection
using Swag

// Returns true if this is a struct of the given 'name'.
#[ConstExpr]
func isStructOfName(type: typeinfo, name: string)->bool
{
    if !isStruct(type) do
        return false

    let typeStruct = cast(const *TypeInfoStruct) type
    return typeStruct.structName == name
}

// Get information about a specific method in a given struct.
// Returns null if the method does not exist
#[ConstExpr]
func getMethod(type: typeinfo, name: string)->#null const *TypeValue
{
    if !isStruct(type) do
        return null

    let typeStruct = cast(const *TypeInfoStruct) type
    foreach it in typeStruct.methods where it.name == name do
        return it
    return null
}

// Get information about a specific field in a given struct
// Returns null if the field does not exist
#[ConstExpr]
func getField(type: typeinfo, name: string)->#null const *TypeValue
{
    if !isStruct(type) do
        return null

    let typeStruct = cast(const *TypeInfoStruct) type
    foreach it in typeStruct.fields where it.name == name do
        return it
    return null
}

// Get a field value with a given runtime type.
#[ConstExpr]
func getFieldValue(dest: *void, type: const *TypeValue, src: *void, hardCast: bool = false)
{
    let patch = cast([*] u8) src + type.offset
    switch type.pointedType
    {
    case s8, u8, bool:
        dref cast(*u8) dest = dref cast(*u8) patch
    case s16, u16:
        dref cast(*u16) dest = dref cast(*u16) patch
    case s32, u32, f32, rune:
        dref cast(*u32) dest = dref cast(*u32) patch
    case s64, u64, f64:
        dref cast(*u64) dest = dref cast(*u64) patch
    case string:
        dref cast(*string) dest = dref cast(*string) patch
    default:
        if !hardCast do
            Debug.assert(false, "unsupported getFieldValue type")
        switch type.pointedType.sizeof
        {
        case 1:
            dref cast(*u8) dest = dref cast(*u8) patch
        case 2:
            dref cast(*u16) dest = dref cast(*u16) patch
        case 4:
            dref cast(*u32) dest = dref cast(*u32) patch
        case 8:
            dref cast(*u64) dest = dref cast(*u64) patch
        default:
            Debug.assert(false, "unsupported getFieldValue type")
        }
    }
}

// Set a field value with a given runtime type.
#[ConstExpr]
func setFieldValue(dest: *void, type: const *TypeValue, src: const *void, hardCast: bool = false)
{
    var patch = cast([*] u8) dest + type.offset
    switch type.pointedType
    {
    case s8, u8, bool:
        dref cast(*u8) patch = dref cast(const *u8) src
    case s16, u16:
        dref cast(*u16) patch = dref cast(const *u16) src
    case s32, u32, f32, rune:
        dref cast(*u32) patch = dref cast(const *u32) src
    case s64, u64, f64:
        dref cast(*u64) patch = dref cast(const *u64) src
    case string:
        dref cast(*string) patch = dref cast(const *string) src
    default:
        if !hardCast do
            Debug.assert(false, "unsupported setFieldValue type")
        switch type.pointedType.sizeof
        {
        case 1:
            dref cast(*u8) patch = dref cast(const *u8) src
        case 2:
            dref cast(*u16) patch = dref cast(const *u16) src
        case 4:
            dref cast(*u32) patch = dref cast(const *u32) src
        case 8:
            dref cast(*u64) patch = dref cast(const *u64) src
        default:
            Debug.assert(false, "unsupported setFieldValue type")
        }
    }
}

// Helper function to know if a struct has a special method
#[ConstExpr]
func hasMethod(type: typeinfo, name: string)->bool => getMethod(type, name) != null

#[ConstExpr, Inline]
{
    func hasDrop(type: typeinfo)->bool     => (type.flags & TypeInfoFlags.HasDrop) ? true : false
    func hasPostMove(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostMove) ? true : false
    func hasPostCopy(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostCopy) ? true : false
    func canCopy(type: typeinfo)->bool     => (type.flags & TypeInfoFlags.CanCopy) ? true : false
    func isPod(type: typeinfo)->bool       => (type.flags & (TypeInfoFlags.HasDrop | TypeInfoFlags.HasPostMove | TypeInfoFlags.HasPostCopy) ? false : true)
}

// Returns true if the interface 'itf' is implemented in the given structure type.
#[ConstExpr]
func hasInterface(type: typeinfo, itf: typeinfo)->bool
{
    if !isStruct(type) do
        return false

    let typeStruct = cast(const *TypeInfoStruct) type
    foreach it in typeStruct.interfaces where it.name == itf.fullname do
        return true
    return false
}
