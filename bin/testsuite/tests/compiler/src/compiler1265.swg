struct Toto
{
    x, y: s32 = 10
}

impl Toto
{
    #[Swag.Implicit]
	#[Swag.Overload]
	func opCast(using self)->s32
	{
        return x + y
	}

    #[Swag.Implicit]
	#[Swag.Overload]
	func opCast(using self)->string
	{
        return "x + y"
	}
}

//#[Swag.PrintBc]
#test
{
	var x: Toto
	let y: s32 = x
	@assert(y == 20)

    let z: string = x
    @assert(z == "x + y")
}