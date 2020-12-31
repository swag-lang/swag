struct Toto
{
    x, y: s32 = 10
}

impl Toto
{
	func opCast(using self)->s32
	{
        return x + y
	}

	func opCast(using self)->string
	{
        return "x + y"
	}
}

//#[swag.printbc]
#test
{
	var x: Toto
	var y: s32 = x
	@assert(y == 20)

    var z: string = x
    @assert(z == "x + y")
}