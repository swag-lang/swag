using swag

#[compiler]
var G = 0

#[compiler]
func toto()
{
    loop 10 G += 1
}

#run
{
    toto()
    @assert(G == 10)
}