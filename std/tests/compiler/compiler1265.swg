struct Toto
{
    x, y: s32 = 10
}

impl Toto
{
	func(T) opCast(using self)->T
	{
        #if @typeof(T) == @typeof(s32)
            return x + y
        #else
            return "x + y"        
	}
}

//#[swag.printbc]
#test
{
	var x: Toto
	var y: s32 = cast(s32) x
	@assert(y == 20)
    
    var z: string = cast(string) x
    @assert(z == "x + y")
}