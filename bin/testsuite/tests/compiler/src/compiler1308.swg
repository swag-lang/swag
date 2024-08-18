#global testerror "assertion failed"

#test
{
	var error = false
    #scope Outer
    loop x in 10
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