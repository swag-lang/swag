#[swag.printbc]
#test
{
    G += 1
    @assert(G == 1)
}

var G: s8 = 0
