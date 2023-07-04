


//#[Swag.PrintBc]
#global testerror
#test
{
	var res = T()
	b := 1
	@print(res.2)
}

//#[Swag.PrintBc]
func T()-> {a,b: s32}
{
	return {1,2}
}
