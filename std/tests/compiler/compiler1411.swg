func toto(x: s32)
{
    #[swag.inline]
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