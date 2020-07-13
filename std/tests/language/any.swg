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

    // 'any' is in fact a pointer to the value, and a const *TypeInfo
    {
        let a: any = 6
        ptr := cast(const *s32) (@dataof(a))
        @assert(deref ptr == 6)
    }

    // '@typeof' will give you 'any', but '@kindof' will give you the real type
    {
        let a: any = "string"
        @assert(@typeof(a) == @typeof(any))
        @assert(@kindof(a) == @typeof(string))
    }
}