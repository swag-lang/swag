#test
{
    // You can get the type of an expression with '@typeof'
    {
        ptr1 := @typeof(s8)
        @assert(ptr1.name == "s8")

        ptr2 := @typeof(s16)
        @assert(ptr2.name == "s16")

        ptr3 := @typeof(s32)
        @assert(ptr3.name == "s32")

        ptr4 := @typeof(s64)
        @assert(ptr4.name == "s64")
    }

    // The return result is a const pointer to a swag.TypeInfo kind of structure
    {
        ptr := @typeof(bool)
        @assert(@typeof(ptr) == @typeof(const *swag.TypeInfoNative))

        ptr1 := @typeof([2] s32)
        @assert(@typeof(ptr1) == @typeof(const *swag.TypeInfoArray))
        @assert(ptr1.name == "[2] s32")
    }

    // The TypeInfo structure contains a different enum value for each type
    {
        ptr := @typeof(f64)
        @assert(@typeof(ptr.kind).name == "swag.TypeInfoKind")
        @assert(ptr.sizeof == @sizeof(f64))
    }
}
