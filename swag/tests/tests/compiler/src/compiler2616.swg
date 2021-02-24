//#global testerror

const G = 1
var G1 = 21

#[swag.compileif(G == 0)]
func toto()
{
    G1 = 0
}

#[swag.compileif(G == 1)]
func toto()
{
    G1 = 1
}


#test
{
    toto()
    @assert(G1 == 1)
}