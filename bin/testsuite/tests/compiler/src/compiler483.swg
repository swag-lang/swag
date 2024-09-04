//#[Swag.PrintBc]
#global testerror "Err0195"
#test
{
	var res = T()
	var b = 1
	@print(res.
	2)
}

//#[Swag.PrintBc]
func T()-> {a,b: s32}
{
	return {1,2}
}
