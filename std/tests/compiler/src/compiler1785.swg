#test
{
    @assert(foo() == 1235)
    //@assert(foo() == 1236)
}

var G = 0

//#[swag.printbc]
func foo()->s32
{
    var S666 = @{ x: 1234's32 }
    S666.x += 1
    G += 1
    return S666.x
}