#global namespace Hash
using Swag

public interface IHash32
{
    mtd const compute()->u32;
}

// Hash the given value and returns the corresponding u32
#[ConstExpr, Safety(.All, .All, false)]
public func(T) hash32(value: T)->u32
{
    #if T == string do
        return Hash.Crc32.compute(value)
    #elif Reflection.isStruct(T) and #defined(T.IHash32) do
        #ast "return " ++ #typeof(T).fullname ++ ".IHash32.compute(value)"
    #elif Reflection.isStruct(T) and #defined(T.IHash64) do
        #ast "return " ++ #typeof(T).fullname ++ ".IHash64.compute(value)"
    #elif Reflection.isStruct(T) and #sizeof(T) == 8 do
        return cast(u32) murmur3(dref cast(const *u64) &value)
    #elif Reflection.isStruct(T) and #sizeof(T) == 4 do
        return murmur3(dref cast(const *u32) &value)
    #elif Reflection.isStruct(T) do
        return Hash.Crc32.compute(cast(const [*] u8) &value, #sizeof(T))
    #elif #sizeof(T) == 8 do
        return cast(u32) murmur3(cast #bit (u64) value)
    #elif #sizeof(T) == 4 do
        return murmur3(cast #bit (u32) value)
    #elif Reflection.isInteger(T) do
        return murmur3(cast(u32) value)
    #else do
        return 0
}
