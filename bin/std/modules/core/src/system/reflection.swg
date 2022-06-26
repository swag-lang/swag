#global public
#global namespace Reflection
using Swag

#[ConstExpr]
{
    #[Inline]
    {
        func isInteger(t: typeinfo)     => t.flags & .Integer
        func isFloat(t: typeinfo)       => t.flags & .Float
        func isType(t: typeinfo)        => t.flags & .TypeInfoPtr
        func isPointer(t: typeinfo)     => t.kind == .Pointer
        func isEnum(t: typeinfo)        => t.kind == .Enum
        func isStruct(t: typeinfo)      => t.kind == .Struct
        func isArray(t: typeinfo)       => t.kind == .Array
        func isSlice(t: typeinfo)       => t.kind == .Slice
        func isTypeAlias(t: typeinfo)   => t.kind == .Alias

        func isRune(t: typeinfo)->bool
        {
            if t.kind != .Native
                return false
            tt := cast(const *TypeInfoNative) t
            return tt.nativeKind == TypeInfoNativeKind.Rune
        }

        func isAny(t: typeinfo)->bool
        {
            if t.kind != .Native
                return false
            tt := cast(const *TypeInfoNative) t
            return tt.nativeKind == TypeInfoNativeKind.Any
        }

        func isBool(t: typeinfo)->bool
        {
            if t.kind != .Native
                return false
            tt := cast(const *TypeInfoNative) t
            return tt.nativeKind == TypeInfoNativeKind.Bool
        }

        func isString(t: typeinfo)->bool
        {
            if t.kind != .Native
                return false
            tt := cast(const *TypeInfoNative) t
            return tt.nativeKind == TypeInfoNativeKind.String
        }
    }

    // Returns true if this is a struct of the given name
    func isStructOfName(type: typeinfo, name: string)->bool
    {
        if type.kind != TypeInfoKind.Struct
            return false
        typeStruct := cast(const* TypeInfoStruct) type
        return typeStruct.structName == name
    }

    // In case this is a type alias, need to go deep inside it to find the right type
    func makeConcreteAlias(type: typeinfo)->typeinfo
    {
        result := type
        while Reflection.isTypeAlias(result)
        {
            typeAlias := cast(const *TypeInfoAlias) result
            result = typeAlias.rawType
        }

        return result
    }

    // Get information about a specific mtd in a given struct
    // Returns null if the mtd does not exist
    func getMethod(type: typeinfo, name: string)->const* TypeValue
    {
        if type.kind != TypeInfoKind.Struct
            return null
        typeStruct := cast(const* TypeInfoStruct) type

        visit it: typeStruct.methods
        {
            if it.name == name
                return it
        }

        return null
    }

    // Get information about a specific field in a given struct
    // Returns null if the field does not exist
    func getField(type: typeinfo, name: string)->const* TypeValue
    {
        if type.kind == TypeInfoKind.Struct
        {
            typeStruct := cast(const* TypeInfoStruct) type
            visit it: typeStruct.fields
            {
                if it.name == name
                    return it
            }
        }

        return null
    }

    // Get a field value with a given runtime type
    func getFieldValue(dest: *void, type: const *TypeValue, src: *void, hardCast: bool = false)
    {
        patch := cast(*u8) src + type.offset
        switch type.pointedType
        {
        case s8, u8, bool:          dref cast(*u8)  dest = dref cast(*u8)  patch
        case s16, u16:              dref cast(*u16) dest = dref cast(*u16) patch
        case s32, u32, f32, rune:   dref cast(*u32) dest = dref cast(*u32) patch
        case s64, u64, f64:         dref cast(*u64) dest = dref cast(*u64) patch
        case string:                dref cast(*string) dest = dref cast(*string) patch
        default:
            if !hardCast
                Debug.assert(false, "unsupported getFieldValue type")
            switch type.pointedType.sizeof
            {
            case 1:     dref cast(*u8)  dest = dref cast(*u8)  patch
            case 2:     dref cast(*u16) dest = dref cast(*u16) patch
            case 4:     dref cast(*u32) dest = dref cast(*u32) patch
            case 8:     dref cast(*u64) dest = dref cast(*u64) patch
            default:    Debug.assert(false, "unsupported getFieldValue type")
            }
        }
    }

    // Set a field value with a given runtime type
    func setFieldValue(dest: *void, type: const *TypeValue, src: const *void, hardCast: bool = false)
    {
        patch := cast(*u8) dest + type.offset
        switch type.pointedType
        {
        case s8, u8, bool:          dref cast(*u8)  patch = dref cast(const *u8)  src
        case s16, u16:              dref cast(*u16) patch = dref cast(const *u16) src
        case s32, u32, f32, rune:   dref cast(*u32) patch = dref cast(const *u32) src
        case s64, u64, f64:         dref cast(*u64) patch = dref cast(const *u64) src
        case string:                dref cast(*string) patch = dref cast(const *string) src
        default:
            if !hardCast
                Debug.assert(false, "unsupported setFieldValue type")
            switch type.pointedType.sizeof
            {
            case 1:     dref cast(*u8)  patch = dref cast(const *u8)  src
            case 2:     dref cast(*u16) patch = dref cast(const *u16) src
            case 4:     dref cast(*u32) patch = dref cast(const *u32) src
            case 8:     dref cast(*u64) patch = dref cast(const *u64) src
            default:    Debug.assert(false, "unsupported setFieldValue type")
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
        if type.kind != TypeInfoKind.Struct
            return false
        typeStruct := cast(const* TypeInfoStruct) type

        visit it: typeStruct.interfaces
        {
            if it.name == itf.name
                return true
        }

        return false
    }

    // Returns the given attribute value
    func getAttributeValue(type: const *Swag.TypeValue, attribute, value: string)->any
    {
        attrib := getAttribute(type, attribute)
        if !attrib
            return null

        visit p: attrib.params
        {
            if p.name == value
                return p.value
        }

        return null
    }

    // Returns true if the given 'type' has the associated 'attribute'
    func getAttribute(type: const *Swag.TypeValue, attribute: string)->const *Swag.Attribute
    {
        visit it: type.attributes
        {
            if attribute == it.name
                return it
        }

        return null
    }

    // Returns true if the given 'type' has the associated 'attribute'
    func getAttribute(type: typeinfo, attribute: string)->const *Swag.Attribute
    {
        switch type.kind
        {
        case TypeInfoKind.Func:
            typeFunc := cast(const *TypeInfoFunc) type
            visit it: typeFunc.attributes
            {
                if attribute == it.name
                    return it
            }

        case TypeInfoKind.Struct:
            typeStruct := cast(const *TypeInfoStruct) type
            visit it: typeStruct.attributes
            {
                if attribute == it.name
                    return it
            }

        case TypeInfoKind.Enum:
            typeEnum := cast(const *TypeInfoEnum) type
            visit it: typeEnum.attributes
            {
                if attribute == it.name
                    return it
            }
        }

        return null
    }

    // Returns true if the given 'type' has the associated 'attribute'
    func hasAttribute(type: typeinfo, attribute: string)->bool
    {
        return getAttribute(type, attribute) != null
    }

    // Get information about a specific value in a given enum
    // Returns null if the value does not exist
    func getEnumValue(type: typeinfo, name: string)->const* TypeValue
    {
        if type.kind == TypeInfoKind.Enum
        {
            typeEnum := cast(const* TypeInfoEnum) type
            visit it: typeEnum.values
            {
                if it.name == name
                    return it
            }
        }

        return null
    }

    // Returns the enum value name corresponding to the value pointed by 'valueAddr'
    // Can have multiple names if the enum is tagged with #[Swag.EnumFlags]
    func getEnumName(type: typeinfo, valueAddr: const* void, fullname = true)->String
    {
        var result: String
        if type.kind != TypeInfoKind.Enum
            return result
        typeEnum := cast(const *TypeInfoEnum) type
        isFlags := hasAttribute(type, "Swag.EnumFlags")

        // Get the user value, as an 'u64' or a string
        var val1: u64
        var val1s: string
        switch typeEnum.rawType.sizeof
        {
        case 1:  val1 = cast(u64) dref cast(const *u8) valueAddr
        case 2:  val1 = cast(u64) dref cast(const *u16) valueAddr
        case 4:  val1 = cast(u64) dref cast(const *u32) valueAddr
        case 8:  val1 = cast(u64) dref cast(const *u64) valueAddr
        case 16: val1s = dref cast(const *string) valueAddr
        default: return result
        }

        simpleName := type.flatName

        visit it: typeEnum.values
        {
            // Get the enum value, as an 'u64' or a string
            var val2: u64
            var val2s: string
            switch typeEnum.rawType.sizeof
            {
            case 1:  val2 = cast(u64) dref cast(const *u8) it.value
            case 2:  val2 = cast(u64) dref cast(const *u16) it.value
            case 4:  val2 = cast(u64) dref cast(const *u32) it.value
            case 8:  val2 = cast(u64) dref cast(const *u64) it.value
            case 16: val2s = dref cast(const *string) it.value
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
                if !result.isEmpty() result += " | "

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
    func maxSizeOf(v: const [..] typeinfo)->uint
    {
        res := 0'uint
        visit p: v
            res = @max(res, p.sizeof)
        return res
    }

    // Make an union of flags of all types
    func orFlags(v: const [..] typeinfo)->Swag.TypeInfoFlags
    {
        var res: Swag.TypeInfoFlags = None
        visit p: v
            res |= p.flags
        return res
    }

    func copy(dst, src: *void, type: typeinfo)
    {
        Memory.copy(dst, src, type.sizeof)
        if hasPostCopy(type)
        {
            ptrStruct := cast(const *Swag.TypeInfoStruct) type
            ptrStruct.opPostCopy(dst)
        }
    }
}
