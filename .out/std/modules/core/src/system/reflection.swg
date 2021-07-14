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
    }

    // Returns true if this is a struct of the given name
    func isStructOfName(type: typeinfo, name: string)->bool
    {
        if type.kind != TypeInfoKind.Struct
            return false
        typeStruct := cast(const* TypeInfoStruct) type
        return typeStruct.structName == name
    }

    // Get information about a specific method in a given struct
    // Returns null if the method does not exist
    func getMethod(type: typeinfo, name: string)->const* TypeInfoParam
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
    func getField(type: typeinfo, name: string)->const* TypeInfoParam
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

    // Helpers functions to know if a struct has a special function
    func hasMethod(type: typeinfo, name: string)->bool => getMethod(type, name) != null

    func hasDrop(type: typeinfo)->bool     => (type.flags & TypeInfoFlags.HasDrop) ? true : false
    func hasReloc(type: typeinfo)->bool    => (type.flags & TypeInfoFlags.HasReloc) ? true : false
    func hasPostMove(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostMove) ? true : false
    func hasPostCopy(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostCopy) ? true : false
    func canCopy(type: typeinfo)->bool     => (type.flags & TypeInfoFlags.CanCopy) ? true : false

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

    // Returns true if the given 'type' has the associated 'attribute'
    func hasAttribute(type: typeinfo, attribute: string)->bool
    {
        switch type.kind
        {
        case TypeInfoKind.Func:
            typeFunc := cast(const *TypeInfoFunc) type
            visit it: typeFunc.attributes
            {
                if attribute == it.name
                    return true
            }

        case TypeInfoKind.Struct:
            typeStruct := cast(const *TypeInfoStruct) type
            visit it: typeStruct.attributes
            {
                if attribute == it.name
                    return true
            }

        case TypeInfoKind.Enum:
            typeEnum := cast(const *TypeInfoEnum) type
            visit it: typeEnum.attributes
            {
                if attribute == it.name
                    return true
            }
        }

        return false
    }

    // Get information about a specific value in a given enum
    // Returns null if the value does not exist
    func getEnumValue(type: typeinfo, name: string)->const* TypeInfoParam
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
    func getEnumName(type: typeinfo, valueAddr: const* void)->String
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
                result += simpleName
                result += "."
                result += it.name
                return result
            }

            // For a flags enum, just append the name if the bit is set
            if isFlags and (val1 & val2)
            {
                if !result.isEmpty() result += " | "
                result += simpleName
                result += "."
                result += it.name
            }
        }

        return result
    }
}
