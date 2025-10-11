#global public
#global namespace Reflection
using Swag

// Transform an enum type to its underling real native type
#[ConstExpr]
func makeConcreteEnum(type: typeinfo)->typeinfo
{
    if !isEnum(type) do
        return type

    let enumType = cast(const *TypeInfoEnum) type
    return enumType.rawType
}

// Get information about a specific value in a given enum
// Returns null if the value does not exist
#[ConstExpr]
func getEnumValue(type: typeinfo, name: string)->#null const *TypeValue
{
    if !isEnum(type) do
        return null

    let typeEnum = cast(const *TypeInfoEnum) type
    foreach it in typeEnum.values where it.name == name do
        return it
    return null
}

// Returns the enum value name corresponding to the value pointed by 'valueAddr'
// Can have multiple names if the enum is tagged with #[Swag.EnumFlags]
#[ConstExpr]
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

    foreach it in typeEnum.values
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
