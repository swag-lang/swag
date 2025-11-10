/*#test
{
    #[Swag.Mixin]
    func MK_IMPL()
    {
        var x: s32 = 1
        var y: s32 = 10
    }

    struct X
    {
        MK_IMPL()
    }


    var v: X
    @assert(v.x == 1)
    @assert(v.y == 10)
}*/
