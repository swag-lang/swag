#test
{
    // 'any' is a specific type that can be every other types
    {
        var a: any
        a = 6
        @assert(a == 6)
        a = "string"
        @assert(a == "string")
        a = true
        @assert(a == true)
    }

    // 'any' is in fact a pointer to the value, and a 'typeinfo'
    {
        var a: any = 6
        ptr := cast(const *s32) @dataof(a)
        @assert(dref ptr == 6)
    }

    // '@typeof' will give you 'any', but '@kindof' will give you the real type
    {
        var a: any = "string"
        @assert(@typeof(a) == any)
        @assert(@kindof(a) == string)
    }

    // You can declare an array with multiple types, with 'any'
    {
        var array: [] any = @[true, 2, 3.0, "4"]
        @assert(@kindof(array[0]) == bool)
        @assert(@kindof(array[1]) == s32)
        @assert(@kindof(array[2]) == f32)
        @assert(@kindof(array[3]) == string)

        @assert(array[0] == true)
        @assert(array[1] == 2)
        @assert(array[2] == 3.0)
        @assert(array[3] == "4")
    }

    // For a function, '@typeof' is the function scoped type, so it cannot be compared
    // with a flat type, but '@kindof' returns the flat type.
    {
        func foo()->s32 => 666
        @assert(@typeof(foo) != func()->s32)
        @assert(@kindof(foo) == func()->s32)
    }
}