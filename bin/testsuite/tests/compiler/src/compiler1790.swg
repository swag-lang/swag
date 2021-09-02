
#global testerror

//#[Swag.PrintBc]
#run
{
    G += 1
}

//#[Swag.PrintBc]
#init
{
    @assert(G == 1)
}

var G: s64
