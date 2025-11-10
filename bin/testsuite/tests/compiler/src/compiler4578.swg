func titi()->s32
{
    return 1
}

#[Swag.Inline]
func toto()->s32
{
    let res = titi()
    return res
}

//#[Swag.PrintBc]
#test
{
    var x: s32
    {
        defer @assert(toto() == 1)
    }
    @assert(x == 0)
}
