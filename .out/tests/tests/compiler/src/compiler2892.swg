#global testerror
//var fn = @(x, y: s32) => x + y

var G = 0

#[swag.macro]
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
