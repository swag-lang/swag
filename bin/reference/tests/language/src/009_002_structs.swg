/**
# Generic Structs
Structs in Swag can also be made generic, allowing them to operate with different types and constants.
*/

#test
{
    {
        // Example of a generic struct where `T` is the generic type.
        struct(T) Struct
        {
            val: T
        }

        let x: Struct's32
        @assert(#typeof(x.val) == s32)         // The type of 'val' is deduced as 's32'.
        let x1: Struct'f32
        @assert(#typeof(x1.val) == f32)        // The type of 'val' is deduced as 'f32'.
    }

    {
        // A more complex example with both a type and a constant as generic parameters.
        struct(T, N: s32) Struct
        {
            val: [N] T  // An array of 'N' elements of type 'T'.
        }

        let x: Struct'(bool, 10)
        @assert(#typeof(x.val) == #type [10] bool) // The type is an array of 10 booleans.
    }
}
