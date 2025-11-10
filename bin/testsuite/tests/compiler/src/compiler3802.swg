struct A {}

impl A
{
    #[Swag.Implicit, Swag.Inline]
    mtd opCast()->s32
    {
        return 666
    }
}

func toto(x: s32)
{
    @assert(x == 666)
}

#test
{
    toto(A{})
    toto(A{})
}
