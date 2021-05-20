
using Swag

func toto1(s = #callerfunction)
{
    @assert(s == "__compiler1017.toto")
}

func toto()
{
	@assert(#function == "__compiler1017.toto")
    toto1()
    #run toto1()
    #run
    {
        toto1()
    }
}

//#[Swag.printbc]
#test
{
	toto()
}
