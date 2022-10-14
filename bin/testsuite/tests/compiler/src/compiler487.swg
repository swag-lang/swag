//#[Swag.PrintBc]
#test
{
	var res = T()
	res1 := res
	@assert(res1.item0 == true)
	@assert(res1.item1 == 2)
	@assert(res1.item2 == false)
}

//#[Swag.PrintBc]
func T()-> {bool,s64,bool}
{
	return @{true,2,false}
}
