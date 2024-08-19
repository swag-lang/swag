#global testerror "Err0140"
//var fn = func(x, y: s32) => x + y

var G = 0

#[Swag.Macro]
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
