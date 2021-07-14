//#[Swag.PrintBc]
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

//#[Swag.PrintBc]
#test
{
	@assert(toto'4() == 1)
}