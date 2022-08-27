	/* salut la foule
		/* */
	*/

func toto()->string
{
	return "toto"
}

#test
{
	x := toto()
	@assert(x == "toto")
}