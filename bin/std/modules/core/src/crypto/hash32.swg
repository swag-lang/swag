#global marked
#global namespace Hash
using Swag

public interface IHash32
{
    mtd const compute()->u32;
}

// Hash the given value and returns the corresponding u32
#[ConstExpr, Safety("", false)]
public func(T) hash32(value: T)->u32
{
    #if T == string:
        return Hash.Crc32.compute(value)
    #elif Reflection.isStruct(T) and #defined(T.IHash32):
        #ast "return " ++ #typeof(T).fullname ++ ".IHash32.compute(value)"
    #elif Reflection.isStruct(T) and #defined(T.IHash64):
        #ast "return " ++ #typeof(T).fullname ++ ".IHash64.compute(value)"
    #elif Reflection.isStruct(T) and #sizeof(T) == 8:
        return cast(u32) murmur3(dref cast(const *u64) &value)
    #elif Reflection.isStruct(T) and #sizeof(T) == 4:
        return murmur3(dref cast(const *u32) &value)
    #elif Reflection.isStruct(T):
        return Hash.Crc32.compute(cast(const ^u8) &value, #sizeof(T))
    #elif #sizeof(T) == 8:
        return cast(u32) murmur3(cast #bit (u64) value)
    #elif #sizeof(T) == 4:
        return murmur3(cast #bit (u32) value)
    #elif Reflection.isInteger(T):
        return murmur3(cast(u32) value)
    #else:
        return 0
}
