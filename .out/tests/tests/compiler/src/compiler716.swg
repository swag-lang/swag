

//#[Swag.printbc]
func(N: s32) toto()->s32
{
	cpt := 0
	for i := 0; i < N; i += 1
	{
		//@print("toto\n")
		cpt += 1
	}

	for i := 0; i < N; i += 1
	{
		//@print("toto\n")
		cpt += 1
	}

	return cpt
}

//#[Swag.printbc]
#test
{
	@assert(toto'4() == 8)
}