#test
{
    #[Swag.Mixin]
    func myMixin(increment: s32)
    {
        a += increment
    }

    var a = 0
    myMixin(1)
    //myMixin(2)
    @assert(a == 1)
}
