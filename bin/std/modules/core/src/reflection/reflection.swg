#global public
#global namespace Reflection
using Swag

#[ConstExpr, Inline]
{
    func isNative(t: nn typeinfo)      => t.kind == .Native
    func isPointer(t: nn typeinfo)     => t.kind == .Pointer
    func isEnum(t: nn typeinfo)        => t.kind == .Enum
    func isStruct(t: nn typeinfo)      => t.kind == .Struct
    func isStaticArray(t: nn typeinfo) => t.kind == .Array
    func isSlice(t: nn typeinfo)       => t.kind == .Slice
    func isTypeAlias(t: nn typeinfo)   => t.kind == .Alias
    func isInterface(t: nn typeinfo)   => t.kind == .Interface

    func isInteger(t: nn typeinfo)           => t.flags & .Integer
    func isFloat(t: nn typeinfo)             => t.flags & .Float
    func isGeneric(t: nn typeinfo)           => t.flags & .Generic
    func isTuple(t: nn typeinfo)             => t.flags & .Tuple
    func isType(t: nn typeinfo)              => t.flags & .PointerTypeInfo
    func isPointerRef(t: nn typeinfo)        => t.flags & .PointerRef
    func isPointerArithmetic(t: nn typeinfo) => t.flags & .PointerArithmetic

    func isEnumFlags(t: nn typeinfo)->bool
    {
        return isEnum(t)
    }

    func isSimpleNative(t: nn typeinfo)->bool
    {
        if !isNative(t):
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind != .Any and tt.nativeKind != .String
    }

    func isRune(t: nn typeinfo)->bool
    {
        if !isNative(t):
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .Rune
    }

    func isAny(t: nn typeinfo)->bool
    {
        if !isNative(t):
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .Any
    }

    func isBool(t: nn typeinfo)->bool
    {
        if !isNative(t):
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .Bool
    }

    func isString(t: nn typeinfo)->bool
    {
        if !isNative(t):
            return false

        let tt = cast(const *TypeInfoNative) t
        return tt.nativeKind == .String
    }
}

#[ConstExpr]
{
    // In case this is a type alias, need to go deep inside it to find the right type
    func makeConcreteAlias(type: nn typeinfo)->typeinfo
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
        foreach p in v:
            res = @max(res, p.sizeof)
        return res
    }

    // Make an union of flags of all types
    func orFlags(v: const [..] typeinfo)->Swag.TypeInfoFlags
    {
        var res: Swag.TypeInfoFlags = Zero
        foreach p in v:
            res |= p.flags
        return res
    }

    func copy(dst, src: nn *void, type: nn typeinfo)
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
    foreach m in @pinfos().modules
    {
        foreach t in m.types
        {
            g_HashCrcToType.add(t.crc, t)
            g_HashNameToType.add(t.fullname, t)
        }
    }
}

// Convert a crc to a registered typeinfo
func crcToType(crc: u32)->typeinfo
{
    Sync.sharedLock(&g_HashLock)
    let e = g_HashCrcToType.find(crc)
    if !e:
        return null
    return e.value
}

// Convert a crc to a registered typeinfo
func nameToType(name: string)->typeinfo
{
    Sync.sharedLock(&g_HashLock)
    let e = g_HashNameToType.find(name)
    if !e:
        return null
    return e.value
}
