public namespace std.reflection;
using swag

#[constexpr]
{
    // Type names are scoped (i.e. "A.B.C"). This function will return the last
    // part, which is in fact the real simple name
    func scopedNameToName(type: typeinfo)->string
    {
        dataof := @dataof(type.name)
        countof := @countof(type.name)
        if !countof return type.name

        buffer := dataof + (countof - 1)
        count := 0'u32
        while buffer != dataof && :buffer != "."'u8
        {
            buffer -= 1
            count += 1
        }

        if buffer == dataof
            return type.name
        return cast(string) @mkslice(buffer + 1, count)
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

    // Helpers functions to know if a struct has a special function
    func hasMethod(type: typeinfo, name: string)->bool => getMethod(type, name) != null

    func hasDrop(type: typeinfo)->bool     => (type.flags & TypeInfoFlags.HasDrop) ? true : false
    func hasPostMove(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostMove) ? true : false
    func hasPostCopy(type: typeinfo)->bool => (type.flags & TypeInfoFlags.HasPostCopy) ? true : false

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

    // Returns the enum value name corresponding to the value pointed by 'valueAddr'
    // Can have multiple names if the enum is tagged with #[swag.enumflags]
    func getEnumName(type: typeinfo, valueAddr: const* void)->String
    {
        var result: String
        if type.kind != TypeInfoKind.Enum
            return result
        typeEnum := cast(const *TypeInfoEnum) type
        isFlags := hasAttribute(type, "swag.enumflags")

        // Get the user value, as an 'u64' or a string
        var val1: u64
        var val1s: string
        switch typeEnum.rawType.sizeof
        {
        case 1:  val1 = cast(u64) :cast(const *u8) valueAddr
        case 2:  val1 = cast(u64) :cast(const *u16) valueAddr
        case 4:  val1 = cast(u64) :cast(const *u32) valueAddr
        case 8:  val1 = cast(u64) :cast(const *u64) valueAddr
        case 16: val1s = :cast(const *string) valueAddr
        default: return result
        }

        simpleName := scopedNameToName(type)

        visit it: typeEnum.values
        {
            // Get the enum value, as an 'u64' or a string
            var val2: u64
            var val2s: string
            switch typeEnum.rawType.sizeof
            {
            case 1:  val2 = cast(u64) :cast(const *u8) it.value
            case 2:  val2 = cast(u64) :cast(const *u16) it.value
            case 4:  val2 = cast(u64) :cast(const *u32) it.value
            case 8:  val2 = cast(u64) :cast(const *u64) it.value
            case 16: val2s = :cast(const *string) it.value
            }

            // For a normal enum, stop if this is a match
            if !isFlags && val1 == val2 && val1s == val2s
            {
                result += simpleName
                result += "."
                result += it.name
                return result
            }

            // For a flags enum, just append the name if the bit is set
            if isFlags && (val1 & val2)
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
