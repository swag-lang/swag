struct(T) Toto
{
    #ast
    {
        type := @typeof(T)
        @assert(type.kind == swag.TypeInfoKind.Struct)
        /*@print(cast(s64) type.kind)
        @print(" ")
        @print(cast(s64) swag.TypeInfoKind.Struct)*/
        //if type.kind != swag.TypeInfoKind.Struct
        //    #assert(false)
        return "x: s32 = 666"
    }
}

//#[swag.printbc]
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