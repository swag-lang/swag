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

    // Transform an enum type to its underling real native type
    func makeConcreteEnum(type: typeinfo)->typeinfo
    {
        if !isEnum(type) do
            return type

        let enumType = cast(const *TypeInfoEnum) type
        return enumType.rawType
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

    // Returns the given attribute value, or null
    #[Swag.Overload]
    func getAttributeValue(type: const *Swag.TypeValue, attribute: typeinfo, value: string)->any
    {
        let attrib = getAttribute(type, attribute)
        if !attrib do
            return null
        return getAttributeValue(attrib, value)
    }

    // Returns the given attribute value, or null
    #[Swag.Overload]
    func(T) getAttributeValue(type: const *Swag.TypeValue, attribute: typeinfo, value: string, def: T)->T
    {
        let attrib = getAttribute(type, attribute)
        if !attrib do
            return def
        let attribVal = getAttributeValue(attrib, value)
        if attribVal == null do
            return def
        return cast(T) attribVal
    }

    // Returns the given attribute value, or null
    #[Swag.Overload]
    func getAttributeValue(type: typeinfo, attribute: typeinfo, value: string)->any
    {
        let attrib = getAttribute(type, attribute)
        if !attrib do
            return null
        return getAttributeValue(attrib, value)
    }

    // Returns the given attribute value, or null
    #[Swag.Overload]
    func getAttributeValue(attribute: const *Swag.Attribute, value: string)->any
    {
        visit p: attribute.params
        {
            if p.name == value do
                return p.value
        }

        return null
    }

    // Returns the given associated attributes to 'value'
    #[Swag.Overload]
    func getAttributes(value: const *Swag.TypeValue, attribute: typeinfo)->Core.Array'(const *Swag.Attribute)
    {
        var result: retval
        visit it: value.attributes
        {
            if attribute == it.type do
                result.add(it)
        }

        return result
    }

    // Returns the given associated 'attribute' to 'value' or null
    #[Swag.Overload]
    func getAttribute(value: const *Swag.TypeValue, attribute: typeinfo)->const *Swag.Attribute
    {
        visit it: value.attributes
        {
            if attribute == it.type do
                return it
        }

        return null
    }

    // Returns true if the given 'value' has the associated 'attribute'
    #[Swag.Overload]
    func hasAttribute(value: const *Swag.TypeValue, attribute: typeinfo)->bool
    {
        return getAttribute(value, attribute) != null
    }

    // Returns the given associated attributes to 'type'
    #[Swag.Overload]
    func getAttributes(type: typeinfo, attribute: typeinfo)->Core.Array'(const *Swag.Attribute)
    {
        var result: retval
        switch type.kind
        {
        case Func:
            let typeFunc = cast(const *TypeInfoFunc) type
            visit it: typeFunc.attributes
            {
                if attribute == it.type do
                    result.add(it)
            }

        case Struct:
            let typeStruct = cast(const *TypeInfoStruct) type
            visit it: typeStruct.attributes
            {
                if attribute == it.type do
                    result.add(it)
            }

        case Enum:
            let typeEnum = cast(const *TypeInfoEnum) type
            visit it: typeEnum.attributes
            {
                if attribute == it.type do
                    result.add(it)
            }
        }

        return result
    }

    // Returns the given associated 'attribute' to 'type' or null
    #[Swag.Overload]
    func getAttribute(type: typeinfo, attribute: typeinfo)->const *Swag.Attribute
    {
        switch type.kind
        {
        case Func:
            let typeFunc = cast(const *TypeInfoFunc) type
            visit it: typeFunc.attributes
            {
                if attribute == it.type do
                    return it
            }

        case Struct:
            let typeStruct = cast(const *TypeInfoStruct) type
            visit it: typeStruct.attributes
            {
                if attribute == it.type do
                    return it
            }

        case Enum:
            let typeEnum = cast(const *TypeInfoEnum) type
            visit it: typeEnum.attributes
            {
                if attribute == it.type do
                    return it
            }
        }

        return null
    }

    // Returns true if the given 'type' has the associated 'attribute'
    #[Swag.Overload]
    func hasAttribute(type: typeinfo, attribute: typeinfo)->bool
    {
        return getAttribute(type, attribute) != null
    }

    // Get information about a specific value in a given enum
    // Returns null if the value does not exist
    func getEnumValue(type: typeinfo, name: string)->const *TypeValue
    {
        if !isEnum(type) do
            return null

        let typeEnum = cast(const *TypeInfoEnum) type
        visit it: typeEnum.values
        {
            if it.name == name do
                return it
        }

        return null
    }

    // Returns the enum value name corresponding to the value pointed by 'valueAddr'
    // Can have multiple names if the enum is tagged with #[Swag.EnumFlags]
    func getEnumName(type: typeinfo, valueAddr: const *void, fullname = true)->String
    {
        var result: String
        if !isEnum(type) do
            return result
        let typeEnum = cast(const *TypeInfoEnum) type
        let isFlags  = hasAttribute(type, EnumFlags)

        // Get the user value, as an 'u64' or a string
        var val1:  u64
        var val1s: string
        switch typeEnum.rawType.sizeof
        {
        case 1:
            val1 = cast(u64) dref cast(const *u8) valueAddr
        case 2:
            val1 = cast(u64) dref cast(const *u16) valueAddr
        case 4:
            val1 = cast(u64) dref cast(const *u32) valueAddr
        case 8:
            val1 = cast(u64) dref cast(const *u64) valueAddr
        case 16:
            val1s = dref cast(const *string) valueAddr
        default:
            return result
        }

        let simpleName = type.name

        visit it: typeEnum.values
        {
            // Get the enum value, as an 'u64' or a string
            var val2:  u64
            var val2s: string
            switch typeEnum.rawType.sizeof
            {
            case 1:
                val2 = cast(u64) dref cast(const *u8) it.value
            case 2:
                val2 = cast(u64) dref cast(const *u16) it.value
            case 4:
                val2 = cast(u64) dref cast(const *u32) it.value
            case 8:
                val2 = cast(u64) dref cast(const *u64) it.value
            case 16:
                val2s = dref cast(const *string) it.value
            }

            // For a normal enum, stop if this is a match
            if !isFlags and val1 == val2 and val1s == val2s
            {
                if fullname
                {
                    result += simpleName
                    result += "."
                }

                result += it.name
                return result
            }

            // For a flags enum, just append the name if the bit is set
            if isFlags and (val1 & val2)
            {
                if !result.isEmpty() do
                    result += " | "

                if fullname
                {
                    result += simpleName
                    result += "."
                }

                result += it.name
            }
        }

        return result
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
