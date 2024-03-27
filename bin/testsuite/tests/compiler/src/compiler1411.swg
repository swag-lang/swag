func toto(x: s32)
{
    #[Swag.Inline]
    func titi(x: s32)->s32
    {
        return x
    }

    @assert(titi(666) == 666)
}

#test
{
    toto(10)
}