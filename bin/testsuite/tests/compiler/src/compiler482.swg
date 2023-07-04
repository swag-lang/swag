


//#[Swag.PrintBc]
#global testerror
#test
{
	var res = T()
	b := 1
	//@print(res.0)
	@print(b.0)
}

//#[Swag.PrintBc]
func T()-> {a,b: s32}
{
	return {1,2}
}
