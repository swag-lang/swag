#global namespace Hash
using Swag

public interface IHash32
{
    mtd const compute()->u32
}

#[Compiler]
public func(T) checkCanHash32()->bool
{
    if @typeof(T) == string do
        return true
    if Reflection.isStruct(T) and !Reflection.hasInterface(T, IHash32) do
        @compilererror("type '" ++ @typeof(T).fullname ++ "' does not implement the 'Hash.IHash32' interface", @location(T))
    if Reflection.isStruct(T) do
        return true
    if @sizeof(T) != 8 and @sizeof(T) != 4 and !Reflection.isInteger(T) do
        @compilererror("cannot hash a value of type '" ++ @typeof(T).fullname ++ "'", @location(T))
    return true
}

// Hash the given value and returns the corresponding u32
#[ConstExpr, Safety("", false)]
public func(T) hash32(value: T)->u32
    #validif checkCanHash32'T()
{
    #if @typeof(T) == string
    {
        return Hash.Crc32.compute(value)
    }
    #elif Reflection.isStruct(T)
    {
        #ast "return " ++ @typeof(T).fullname ++ ".IHash32.compute(value)"
    }
    #elif @sizeof(T) == 8
    {
        let val64 = cast,bit(u64) value
        var key = (~val64) + (val64 << 18)
        key = key ^ (key >> 31)
        key = key * 21
        key = key ^ (key >> 11)
        key = key + (key << 6)
        key = key ^ (key >> 22)
        return cast(u32) key
    }
    #elif @sizeof(T) == 4
    {
        let val32 = cast,bit(u32) value
        var key = (~val32) + (val32 << 15)
        key = key ^ (key >> 12)
        key = key + (key << 2)
        key = key ^ (key >> 4)
        key = key * 2057
        key = key ^ (key >> 16)
        return key
    }
    #elif Reflection.isInteger(T)
    {
        let val32 = cast(u32) value
        var key = (~val32) + (val32 << 15)
        key = key ^ (key >> 12)
        key = key + (key << 2)
        key = key ^ (key >> 4)
        key = key * 2057
        key = key ^ (key >> 16)
        return key
    }
}
