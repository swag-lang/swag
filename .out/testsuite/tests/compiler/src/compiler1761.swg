#test
{
    #[Swag.Mixin]
    func myMixin(value: s32, increment: s32)
    {
        a += increment
    }

    a := 0
    myMixin(0,1)
    myMixin(0,2)
    @assert(a == 3)
}
