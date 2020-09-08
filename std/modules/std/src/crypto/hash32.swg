namespace std.crypto
{
    #[swag.constexpr]
    public func(T) hash32(value: T)->u32
    {
        #if @typeof(T).flags & swag.TypeInfoFlags.Integer
            return cast(u32) value
        #else
            #assert(false, "cannot hash a value of type '" ~ @typeof(T).name ~ "'")
    }
}