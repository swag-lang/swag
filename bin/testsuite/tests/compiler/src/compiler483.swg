


//#[Swag.PrintBc]
#global testerror
#test
{
	var res = T()
	b := 1
	@print(res.2)
}

//#[Swag.PrintBc]
func T()-> {s32, s32}
{
	return {1,2}
}
