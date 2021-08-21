#global testerror

#test
{
	error := false
    label outer
    loop x: 10
    {
        label outer
        loop 20
        {
            break outer
        }

		error = true
    }

	@assert(!error)
}