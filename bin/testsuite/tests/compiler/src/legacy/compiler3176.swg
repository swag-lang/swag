//#global skip

#[Swag.Inline]
func toto()->s32
{
    #[Swag.Mixin]
    func x()->s32 { return 666; }
    return x()
}

#test
{
    @assert(toto() == 666)
}
