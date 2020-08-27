//#[swag.printbc]
func(N: s32) toto()->s32
{
	cpt := 0
	loop(N)
	{
		//@print("toto\n")
		cpt += 1
		break
	}

	return cpt
}

//#[swag.printbc]
#test
{
	@assert(toto'4() == 1)
}