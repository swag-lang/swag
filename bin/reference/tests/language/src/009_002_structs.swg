/**
# Generic Structs
Structs in Swag can also be made generic, allowing them to operate with different types and constants.
*/
#test
{
    {
        // Generic struct example where 'T' represents a type parameter.
        struct(T) Struct
        {
            val: T
        }

        let x: Struct's32
        @assert(#typeof(x.val) == s32) // The field 'val' has type 's32'.

        let x1: Struct'f32
        @assert(#typeof(x1.val) == f32) // The field 'val' has type 'f32'.
    }

    {
        // Generic struct with both a type and a constant parameter.
        struct(T, N: s32) Struct
        {
            val: [N] T // Array of 'N' elements of type 'T'.
        }

        let x: Struct'(bool, 10)
        @assert(#typeof(x.val) == #type [10] bool) // 'val' is an array of 10 booleans.
    }
}
