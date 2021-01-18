#test
{
    {
        // Like functions, a struct can be generic
        struct(T) Struct
        {
            val: T
        }

        var x: Struct's32
        @assert(@typeof(x.val) == s32)
        var x1: Struct'f32
        @assert(@typeof(x1.val) == f32)
    }

    {
        // Like functions, a struct can be generic
        struct(T, N: s32) Struct
        {
            val: [N] T
        }

        var x: Struct'(bool, 10)
        @assert(@typeof(x.val) == [10] bool)
    }
}
