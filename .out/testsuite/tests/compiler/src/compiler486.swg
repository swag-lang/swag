


//#[Swag.printbc]
#test
{
	var res = T()
	@assert(res.item0 == true)
	@assert(res.item1 == 2)
	@assert(res.item2 == false)
}

//#[Swag.printbc]
func T()-> {bool,s32,bool}
{
	return @{true,2,false}
}
