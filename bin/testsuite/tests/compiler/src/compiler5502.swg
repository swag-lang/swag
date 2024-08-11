#global testerror "Err0127"
//var fn = func(x, y: s32) => x + y

var G = 0

#[Swag.Mixin]
func toto()
{
    G = 1
}

var fn = &toto
#test
{
    fn()
    @assert(G == 1)
}
