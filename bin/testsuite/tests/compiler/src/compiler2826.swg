#global testerror "Err0530"

struct Toto
{
}

impl Toto
{
    //#[Swag.Implicit]
	func opCast(using self)->s32
	{
		return 666
	}
}

#test
{
	var x: Toto
	var y: s32 = x
	@assert(y == 666)
}