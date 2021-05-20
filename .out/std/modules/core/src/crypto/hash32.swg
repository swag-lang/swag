#global namespace Hash
using Swag

// Hash the given value and returns the corresponding u32
#[constexpr]
public func(T) hash32(value: T)->u32
{
    #if reflection.isInteger(T)
    {
        #[Swag.safety("overflow", false)]
        return cast(u32) value
    }
    #elif reflection.isFloat(T)
    {
        val32 := cast(f32) value
        return bitcast(u32) val32
    }
    #elif @typeof(T) == string
    {
        return Hash.Crc32.compute(value)
    }
    #elif reflection.isStruct(T)
    {
        #assert(reflection.hasInterface(T, IHash32), "type '" ~ @typeof(T).name ~ "' does not implement the 'Hash.IHash32' interface")
        #ast "return " ~ @typeof(T).name ~ ".IHash32.compute(value)"
    }
    #else
        #assert(false, "cannot hash a value of type '" ~ @typeof(T).name ~ "'")
}
