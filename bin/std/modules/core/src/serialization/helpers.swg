#global namespace Serialization

// Determin if a type can be serialized by a simple copy
public func isRawSerializable(type: typeinfo)->{raw: bool, sizeof: uint}
{
    if Reflection.isInteger(type) or
       Reflection.isFloat(type) or
       Reflection.isBool(type) or
       Reflection.isRune(type)
    {
        return @{true, type.sizeof}
    }

    return @{false, 0}
}

public func getCrc(name: string)->u32
{
    return Hash.Crc32.compute(name)
}