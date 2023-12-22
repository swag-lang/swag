#global namespace Serialization

// Determin if a type can be serialized by a simple copy
public func isPodFinal(type: typeinfo)->{raw: bool, sizeof: u64}
{
    if Reflection.isInteger(type) or
       Reflection.isFloat(type) or
       Reflection.isBool(type) or
       Reflection.isRune(type)
    {
        return {true, type.sizeof}
    }

    if Reflection.hasAttribute(type, Serialization.PodFinal) do
        return {true, type.sizeof}

    return {false, 0}
}