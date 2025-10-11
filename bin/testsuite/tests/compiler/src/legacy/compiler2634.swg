using Swag

#[Compiler]
var G = 0

#[Compiler]
func toto()
{
    for 10 do
        G += 1
}

#run
{
    toto()
    @assert(G == 10)
}
