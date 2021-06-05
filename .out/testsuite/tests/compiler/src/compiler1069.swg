

struct titi
{
	buf: [4] u8;
}

impl titi
{
	func toString(using self) -> const [..] u8
	{
		return @mkslice(&buf[0], 4)
	}
}

//#[Swag.printbc]
#test
{
	var t: titi
	t.buf[0] = 1
	t.buf[1] = 2
	t.buf[2] = 3
	t.buf[3] = 4

	var x = t.toString()
	@assert(x[0] == 1)
	@assert(x[1] == 2)
	@assert(x[2] == 3)
	@assert(x[3] == 4)
}