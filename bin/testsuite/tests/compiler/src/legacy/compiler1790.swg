#global testerror "Err0127"

//#[Swag.PrintBc]
#run
{
    G += 1
}

//#[Swag.PrintBc]
#test
{
    @assert(G == 1)
}

var G: s64
