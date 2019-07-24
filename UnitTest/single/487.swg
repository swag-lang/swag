#module module487
//#unittest pass semantic
/*
#[swag.printbytecode]
#test
{
	var res = T()
	res1 := res
	//@assert(res1.0 == true)
	//@assert(res1.1 == 2)
	//@assert(res1.2 == false)
}

//#[swag.printbytecode]
func T()-> {bool,s64,bool}
{
	return {true,2,false}
}
*/