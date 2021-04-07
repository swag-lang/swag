#global testerror

struct(T) Toto
{
}

impl Toto
{
    #[swag.implicit]
	func opCast(using self)->s32
	{
		return 666
	}
}

#test
{
	var x: Toto's32
	var y: s32 = x
	@assert(y == 666)
}