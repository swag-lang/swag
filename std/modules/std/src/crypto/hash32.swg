namespace hash;
using swag

public interface IHash32
{
    compute: func(self)->u32
}

// Hash the given value and returns the corresponding u32
#[constexpr]
public func(T) hash32(value: T)->u32
{
    #if @typeof(T).flags & TypeInfoFlags.Integer
    {
        return cast(u32) value
    }
    #elif @typeof(T).flags & TypeInfoFlags.Float
    {
        val32 := cast(f32) value
        return bitcast(u32) val32
    }
    #elif @typeof(T) == string
    {
        return hash.Crc32.compute(value)
    }
    #elif @typeof(T).kind == TypeInfoKind.Struct
    {
        #assert(reflection.hasInterface(@typeof(T), IHash32), "type '" ~ @typeof(T).name ~ "' does not implement the 'hash.IHash32' interface")
        #ast "return " ~ @typeof(T).name ~ ".IHash32.compute(value)"
    }
    #else
        #assert(false, "cannot hash a value of type '" ~ @typeof(T).name ~ "'")
}
