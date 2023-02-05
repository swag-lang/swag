#global testerror

#test
{
	error := false
    #scope Outer
    loop x: 10
    {
        #scope Outer
        loop 20
        {
            break Outer
        }

		error = true
    }

	@assert(!error)
}