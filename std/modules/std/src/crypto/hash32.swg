namespace std.crypto
{
    using swag

    public interface IHash32
    {
        hash: func(self)->u32
    }

    // Hash the given value and returns the corresponding u32
    #[swag.constexpr]
    public func(T) hash32(value: T)->u32
    {
        #if @typeof(T).flags & TypeInfoFlags.Integer
        {
            return cast(u32) value
        }
        #elif @typeof(T).flags & TypeInfoFlags.Float
        {
            val32 := cast(f32) value
            return deref cast(const *u32) &val32
        }
        #elif @typeof(T) == string
        {
            return crc32(value)
        }
        #elif @typeof(T).kind == TypeInfoKind.Struct
        {
            itf := @mkinterface(value, @typeof(T), IHash32)
            if itf != null
                return itf.hash()
            return 0
        }
        #else
            #assert(false, "cannot hash a value of type '" ~ @typeof(T).name ~ "'")
    }
}