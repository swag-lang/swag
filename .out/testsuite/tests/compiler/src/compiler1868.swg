struct(T) Toto
{
    #ast
    {
        type := @typeof(T)
        @assert(type.kind == Swag.TypeInfoKind.Struct)
        /*@print(cast(s64) type.kind)
        @print(" ")
        @print(cast(s64) Swag.TypeInfoKind.Struct)*/
        //if type.kind != Swag.TypeInfoKind.Struct
        //    #assert(false)
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
    //var gen1: Toto's32
}