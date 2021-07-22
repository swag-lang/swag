#global export
#global namespace Math

struct(T: const [..] typeinfo) Variant
{
    type:   typeinfo
    buffer: [Reflection.maxSizeOf(T)] u8
}

impl Variant
{
    method(TN) set(value: TN)
    {
        // Be sure type is supported
        #assert(Slice.contains(T, @typeof(TN)), "type '" ~@stringof(TN)~ "' is not part of the variant")
        type = TN
        dref cast(*TN) buffer = value
    }

    method(TN) get()->TN
    {
        // Be sure type is supported
        #assert(Slice.contains(T, @typeof(TN)), "type '" ~@stringof(TN)~ "' is not part of the variant")
        Debug.assert(type == TN, "invalid type")
        return dref cast(*TN) buffer
    }
}