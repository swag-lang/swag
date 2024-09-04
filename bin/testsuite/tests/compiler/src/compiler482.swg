#global testerror "Err0195"
#test
{
	var res = T()
	var b = 1
	//@print(res.0)
	@print(b.0)
}

//#[Swag.PrintBc]
func T()-> {a,b: s32}
{
	return {1,2}
}
