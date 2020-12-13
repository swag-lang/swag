
#unittest error

//#[swag.printbc]
#run
{
    G += 1
}

//#[swag.printbc]
#init
{
    @assert(G == 1)
}

var G: s64
