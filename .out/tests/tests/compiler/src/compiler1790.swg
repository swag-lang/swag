
#global testerror

//#[Swag.printbc]
#run
{
    G += 1
}

//#[Swag.printbc]
#init
{
    @assert(G == 1)
}

var G: s64
