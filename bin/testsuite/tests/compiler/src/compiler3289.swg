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

    #assert #defined(X.x)
    #assert #defined(X.y)
    #assert !#defined(X.z)
    #assert #sizeof(X) == 8

    var v: X
    @assert(v.x == 1)
    @assert(v.y == 10)
}*/