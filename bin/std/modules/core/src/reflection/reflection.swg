#global public
#global namespace Reflection
using Swag

#[ConstExpr]
{
    #[Inline]
    {
        func isNative(t: typeinfo)    => t.kind == .Native
        func isPointer(t: typeinfo)   => t.kind == .Pointer
        func isEnum(t: typeinfo)      => t.kind == .Enum
        func isStruct(t: typeinfo)    => t.kind == .Struct
        func isArray(t: typeinfo)     => t.kind == .Array
        func isSlice(t: typeinfo)     => t.kind == .Slice
        func isTypeAlias(t: typeinfo) => t.kind == .Alias
        func isInterface(t: typeinfo) => t.kind == .Interface

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

    // Returns true if this is a struct of the given name
    func isStructOfName(type: typeinfo, name: string)->bool
    {
        if !isStruct(type) do
            return false

        let typeStruct = cast(const *TypeInfoStruct) type
        return typeStruct.structName == name
    }

    // In case this is a type typealias, need to go deep inside it to find the right type
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

    // Get information about a specific mtd in a given struct
    // Returns null if the mtd does not exist
    func getMethod(type: typeinfo, name: string)->const *TypeValue
    {
        if !isStruct(type) do
            return null

        let typeStruct = cast(const *TypeInfoStruct) type
        visit it: typeStruct.methods
        {
            if it.name == name do
                return it
        }

        return null
    }

    // Get information about a specific field in a given struct
    // Returns null if the field does not exist
    func getField(type: typeinfo, name: string)->const *TypeValue
    {
        if !isStruct(type) do
            return null

        let typeStruct = cast(const *TypeInfoStruct) type
        visit it: typeStruct.fields
        {
            if it.name == name do
                return it
        }

        return null
    }

    // Get a field value with a given runtime type
    func getFieldValue(dest: *void, type: const *TypeValue, src: *void, hardCast: bool = false)
    {
        let patch = cast(^u8) src + type.offset
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

    // Set a field value with a given runtime type
    func setFieldValue(dest: *void, type: const *TypeValue, src: const *void, hardCast: bool = false)
    {
        var patch = cast(^u8) dest + type.offset
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

    // Helpers functions to know if a struct has a special function
    func hasMethod(type: typeinfo, name: string)->bool => getMethod(type, name) != null

    #[Inline]
    {
        func hasDrop(type: typeinfo)->bool     => (type.flags & TypeInfoFlags.HasDrop) ? true : false
        func hasPostMove(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostMove) ? true : false
        func hasPostCopy(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostCopy) ? true : false
        func canCopy(type: typeinfo)->bool     => (type.flags & TypeInfoFlags.CanCopy) ? true : false
    }

    // Returns true if the interface 'itf' is implemented in the given structure type
    func hasInterface(type: typeinfo, itf: typeinfo)->bool
    {
        if !isStruct(type) do
            return false

        let typeStruct = cast(const *TypeInfoStruct) type
        visit it: typeStruct.interfaces
        {
            if it.name == itf.fullname do
                return true
        }

        return false
    }

    // Returns the maximum sizeof a bunch of 'typeinfo'
    func maxSizeOf(v: const [..] typeinfo)->u64
    {
        var res = 0'u32
        visit p: v do
            res = @max(res, p.sizeof)
        return res
    }

    // Make an union of flags of all types
    func orFlags(v: const [..] typeinfo)->Swag.TypeInfoFlags
    {
        var res: Swag.TypeInfoFlags = Zero
        visit p: v do
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
    visit m: @pinfos().modules
    {
        visit t: m.types
        {
            g_HashCrcToType.add(t.crc32, t)
            g_HashNameToType.add(t.fullname, t)
        }
    }
}

// Convert a crc to a registered typeinfo
func crcToType(crc: u32)->typeinfo
{
    Sync.sharedLock(&g_HashLock)
    let e = g_HashCrcToType.find(crc)
    if !e do
        return null
    return e.value
}

// Convert a crc to a registered typeinfo
func nameToType(name: string)->typeinfo
{
    Sync.sharedLock(&g_HashLock)
    let e = g_HashNameToType.find(name)
    if !e do
        return null
    return e.value
}
