#global public
#global namespace Reflection
using Swag

#[ConstExpr, Inline]
{
    func isNative(t: typeinfo)      => t.kind == .Native
    func isPointer(t: typeinfo)     => t.kind == .Pointer
    func isEnum(t: typeinfo)        => t.kind == .Enum
    func isStruct(t: typeinfo)      => t.kind == .Struct
    func isStaticArray(t: typeinfo) => t.kind == .Array
    func isSlice(t: typeinfo)       => t.kind == .Slice
    func isTypeAlias(t: typeinfo)   => t.kind == .Alias
    func isInterface(t: typeinfo)   => t.kind == .Interface

    func isInteger(t: typeinfo)           => t.flags & .Integer
    func isFloat(t: typeinfo)             => t.flags & .Float
    func isGeneric(t: typeinfo)           => t.flags & .Generic
    func isTuple(t: typeinfo)             => t.flags & .Tuple
    func isType(t: typeinfo)              => t.flags & .PointerTypeInfo
    func isPointerRef(t: typeinfo)        => t.flags & .PointerRef
    func isPointerArithmetic(t: typeinfo) => t.flags & .PointerArithmetic

    func isEnumFlags(t: typeinfo)->bool
    {
        return isEnum(t)
    }

    func isSimpleNative(t: typeinfo)->bool
    {
        if !isNative(t) do
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind != .Any and tt.nativeKind != .String
    }

    func isRune(t: typeinfo)->bool
    {
        if !isNative(t) do
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .Rune
    }

    func isAny(t: typeinfo)->bool
    {
        if !isNative(t) do
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .Any
    }

    func isBool(t: typeinfo)->bool
    {
        if !isNative(t) do
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .Bool
    }

    func isString(t: typeinfo)->bool
    {
        if !isNative(t) do
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .String
    }
}

#[ConstExpr]
{
    // In case this is a type alias, need to go deep inside it to find the right type
    func makeConcreteAlias(type: typeinfo)->typeinfo
    {
        var result = type
        while isTypeAlias(result)
        {
            let typeAlias = cast(const *TypeInfoAlias) result
            result = typeAlias.rawType
        }

        return result
    }

    // Returns the maximum sizeof a bunch of 'typeinfo'
    func maxSizeOf(v: const [..] typeinfo)->u64
    {
        var res = 0'u32
        foreach p in v do
            res = @max(res, p.sizeof)
        return res
    }

    // Make an union of flags of all types
    func orFlags(v: const [..] typeinfo)->Swag.TypeInfoFlags
    {
        var res: Swag.TypeInfoFlags = Zero
        foreach p in v do
            res |= p.flags
        return res
    }

    func copy(dst, src: *void, type: typeinfo)
    {
        Memory.copy(dst, src, cast(u64) type.sizeof)
        if hasPostCopy(type)
        {
            let ptrStruct = cast(const *Swag.TypeInfoStruct) type
            ptrStruct.opPostCopy(dst)
        }
    }
}

internal var g_HashCrcToType: HashTable'(u32, typeinfo)
internal var g_HashNameToType: HashTable'(string, typeinfo)
internal var g_HashLock: Sync.RWLock

// #premain can be called more than once if we compile/run more than one
// module using Core at the same time. So lock !
#premain
{
    Sync.scopedLock(&g_HashLock)
    foreach m in @pinfos.modules
    {
        foreach t in m.types
        {
            g_HashCrcToType.add(t.crc, t)
            g_HashNameToType.add(t.fullname, t)
        }
    }
}

// Convert a crc to a registered typeinfo
func crcToType(crc: u32)->#null typeinfo
{
    Sync.sharedLock(&g_HashLock)
    let e = g_HashCrcToType.tryFind(crc)
    if !e do
        return null
    return e.value
}

// Convert a crc to a registered typeinfo
func nameToType(name: string)->#null typeinfo
{
    Sync.sharedLock(&g_HashLock)
    let e = g_HashNameToType.tryFind(name)
    if !e do
        return null
    return e.value
}
