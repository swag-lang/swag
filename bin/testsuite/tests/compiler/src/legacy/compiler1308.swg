#global testerror "assertion failed"

#test
{
	var error = false
    #scope(Outer)
    for x in 10
    {
        #scope(Outer)
        for 20
        {
            break to Outer
        }

		error = true
    }

	@assert(!error)
}