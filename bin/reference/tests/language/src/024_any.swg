/**
`any` is a specific type that can store every other types. `any` is **not a variant**. It's a dynamic typed reference to an existing value.
*/

#test
{
    {
        var a: any

        // Store a s32
        a = 6
        @assert(a == 6)

        // Then store a string instead
        a = "string"
        @assert(a == "string")

        // Then store a bool instead
        a = true
        @assert(a == true)
    }

    // `any` is in fact a pointer to the value, and a `typeinfo`. `@dataof` can be used to retrieve the pointer to the value.
    {
        let a: any = 6
        let ptr = cast(const *s32) @dataof(a)
        @assert(dref ptr == 6)
    }

    // `@typeof` will give you the type `any`, but `@kindof` will give you the real underlying type. In that case, `@kindof` is evaluted at runtime.
    {
        var a: any = "string"
        #assert @typeof(a) == any
        @assert(@kindof(a) == string)

        a = true
        @assert(@kindof(a) == bool)
    }

    // You can declare an array with multiple types, with `any`.
    {
        var array: [] any = [true, 2, 3.0, "4"]
        @assert(@kindof(array[0]) == bool)
        @assert(@kindof(array[1]) == s32)
        @assert(@kindof(array[2]) == f32)
        @assert(@kindof(array[3]) == string)

        @assert(array[0] == true)
        @assert(array[1] == 2)
        @assert(array[2] == 3.0)
        @assert(array[3] == "4")
    }

    // An `any` can be set to null, and tested against null.
    {
        var x: any
        @assert(x == null)
        x = 6
        @assert(x != null)
        @assert(x == 6)
        x = "string"
        @assert(x != null)
        @assert(x == "string")
        x = null
        @assert(x == null)
    }
}