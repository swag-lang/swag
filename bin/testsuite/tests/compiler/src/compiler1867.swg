struct(T) Toto
{
    #ast
    {
        //var type = #typeof(T)
        return "x: s32 = 666"
    }
}

//#[Swag.PrintBc]
#test
{
    struct MyStruct
    {
        x: f32
        y: bool
    }

    var gen: Toto'MyStruct
    @assert(cast(s64) gen.x == 666)
}