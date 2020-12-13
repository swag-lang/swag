
#unittest error

#test
{
	error := false
    outer: loop x: 10
    {
        outer: loop 20
        {
            break outer
        }

		error = true
    }

	@assert(!error)
}