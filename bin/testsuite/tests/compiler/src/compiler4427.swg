using Swag

var G = 5

func toto(x: &*s32)
{
    x = &G
}

#test
{
    var a   = 10
    var ptr = &a
    toto(&ptr)

    //@print(dref ptr)
    @assert(dref ptr == 5)
}
