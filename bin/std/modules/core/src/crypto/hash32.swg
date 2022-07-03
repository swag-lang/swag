#global namespace Hash
using Swag

public interface IHash32
{
    compute: func(const self)->u32
}

// Hash the given value and returns the corresponding u32
#[ConstExpr, Safety("", false)]
public func(T) hash32(value: T)->u32
{
    #if @typeof(T) == string
    {
        return Hash.Crc32.compute(value)
    }
    #elif Reflection.isStruct(T)
    {
        #assert(Reflection.hasInterface(T, IHash32), "type '" ++ @typeof(T).fullname ++ "' does not implement the 'Hash.IHash32' interface")
        #ast "return " ++ @typeof(T).fullname ++ ".IHash32.compute(value)"
    }
    #elif @sizeof(T) == 8
    {
        val64 := bitcast(u64) value
        key := (~val64) + (val64 << 18)
        key = key ^ (key >> 31)
        key = key * 21
        key = key ^ (key >> 11)
        key = key + (key << 6)
        key = key ^ (key >> 22)
        return cast(u32) key
    }
    #elif @sizeof(T) == 4
    {
        val32 := bitcast(u32) value
        key := (~val32) + (val32 << 15)
        key = key ^ (key >> 12)
        key = key + (key << 2)
        key = key ^ (key >> 4)
        key = key * 2057
        key = key ^ (key >> 16)
        return key
    }
    #elif Reflection.isInteger(T)
    {
        val32 := cast(u32) value
        key := (~val32) + (val32 << 15)
        key = key ^ (key >> 12)
        key = key + (key << 2)
        key = key ^ (key >> 4)
        key = key * 2057
        key = key ^ (key >> 16)
        return key
    }
    #else
        #assert(false, "cannot hash a value of type '" ++ @typeof(T).fullname ++ "'")
}
