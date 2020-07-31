#[swag.printbc]
#test
{
    G += 1
    @assert(G == 1)
}

var G = 0
