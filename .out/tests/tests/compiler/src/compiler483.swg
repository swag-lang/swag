


//#[Swag.printbc]
#global testerror
#test
{
	var res = T()
	b := 1
	@print(res.2)
}

//#[Swag.printbc]
func T()-> {s32, s32}
{
	return {1,2}
}
