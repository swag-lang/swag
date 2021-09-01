#global testerror

#test
{
	error := false
    scope outer
    loop x: 10
    {
        scope outer
        loop 20
        {
            break outer
        }

		error = true
    }

	@assert(!error)
}