struct Toto
{
    x, y: s32 = 10
}

impl Toto
{
    #[Swag.implicit]
	func opCast(using self)->s32
	{
        return x + y
	}

    #[Swag.implicit]
	func opCast(using self)->string
	{
        return "x + y"
	}
}

//#[Swag.PrintBc]
#test
{
	var x: Toto
	var y: s32 = x
	@assert(y == 20)

    var z: string = x
    @assert(z == "x + y")
}