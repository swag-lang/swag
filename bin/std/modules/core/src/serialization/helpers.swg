#global namespace Serialization

// Determin if a type can be serialized by a simple copy
public func isPodFinal(type: #null typeinfo)->{ raw: bool, sizeof: u64 }
{
    if type != null
    {
        if Reflection.isInteger(type) or
           Reflection.isFloat(type) or
           Reflection.isBool(type) or
           Reflection.isRune(type)
        {
            return {true, type.sizeof}
        }

        if !Reflection.isPod(type) do
            return {false, 0}

        if Reflection.hasAttribute(type, Serialization.Final) do
            return {true, type.sizeof}
    }

    return {false, 0}
}
