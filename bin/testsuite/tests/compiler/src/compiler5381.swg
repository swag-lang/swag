struct A {}
impl A
{
    #[Swag.Overload]
    mtd opIndexAffect(x: string, v: s32)
    {
        @assert(x == "a")
        @assert(v == 42)
    }

    #[Swag.Overload]
    mtd opIndexAffect(x: s32, v: s32)
    {
        @assert(x == 42)
        @assert(v == 43)
    }
}

#test
{
    var a: A
    a["a"] = 42
    a[42]  = 43
}
