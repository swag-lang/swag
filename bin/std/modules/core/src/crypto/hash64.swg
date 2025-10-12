#global namespace Hash
using Swag

public interface IHash64
{
    mtd const compute()->u64;
}

// Hash the given value and returns the corresponding u64
#[ConstExpr, Safety(.All, .All, false)]
public func(T) hash64(value: T)->u64
{
    #if T == string do
        return Hash.murmur3(@dataof(value), @countof(value), 0)
    #elif Reflection.isStruct(T) and #defined(T.IHash64) do
        #ast "return " ++ #typeof(T).fullname ++ ".IHash64.compute(value)"
    #elif Reflection.isStruct(T) and #defined(T.IHash32) do
        #ast "return " ++ #typeof(T).fullname ++ ".IHash32.compute(value)"
    #elif Reflection.isStruct(T) do
        return Hash.murmur3(cast(const [*] u8) &value, #sizeof(T), 0)
    #elif #sizeof(T) == 8 do
        return murmur3(cast #bit (u64) value)
    #elif #sizeof(T) == 4 do
        return murmur3(cast #bit (u32) value)
    #elif Reflection.isInteger(T) do
        return murmur3(cast(u64) value)
    #else do
        return 0
}
