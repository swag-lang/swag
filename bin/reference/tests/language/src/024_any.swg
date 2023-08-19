/**
`any` is a specific type that can store every other types. `any` is **not a variant**. It's a dynamic typed reference to an existing value.
*/

#test
{
    var a: any

    // Store a 's32' literal value
    a = 6

    // In order to reference the value inside the 'any', you need to cast to the wanted type.
    @assert(cast(s32) a == 6)

    // Then now we store a string instead of the 's32' value
    a = "string"
    @assert(cast(string) a == "string")

    // Then we store a bool instead
    a = true
    @assert(cast(bool) a == true)
}

/**
`any` is in fact a pointer to the value, and a `typeinfo`. `@dataof` can be used to retrieve the pointer to the value.
*/
#test
{
    let a: any = 6
    let ptr = cast(const *s32) @dataof(a)
    @assert(dref ptr == 6)
}

/**
`@typeof` will give you the type `any`, but `@kindof` will give you the real underlying type. In that case, `@kindof` is evaluted at runtime.
*/
#test
{
    var a: any = "string"
    #assert @typeof(a) == any
    @assert(@kindof(a) == string)

    a = true
    @assert(@kindof(a) == bool)
}

/**
You can declare an array with multiple types, with `any`.
*/
#test
{
    var array: [] any = [true, 2, 3.0, "4"]
    @assert(@kindof(array[0]) == bool)
    @assert(@kindof(array[1]) == s32)
    @assert(@kindof(array[2]) == f32)
    @assert(@kindof(array[3]) == string)

    @assert(cast(bool) array[0] == true)
    @assert(cast(s32) array[1] == 2)
    @assert(cast(f32) array[2] == 3.0)
    @assert(cast(string) array[3] == "4")
}

/**
An `any` can be set to null, and tested against null.
*/
#test
{
    var x: any
    @assert(x == null)
    x = 6
    @assert(x != null)
    @assert(cast(s32) x == 6)
    x = "string"
    @assert(x != null)
    @assert(cast(string) x == "string")
    x = null
    @assert(x == null)
}
