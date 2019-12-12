func toto(x: s32)
{
    #[swag.inline]
    func titi(x: s32)->s32
    {
        return x
    }
    
    s := titi(0)
}
