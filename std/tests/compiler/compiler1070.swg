

struct titi
{
	buf: [4] u8;
}

impl titi
{
	func toString(using self) -> const [..] u8
	{
		return [&buf[0], 4]
	}
}

func toto(v: string)
{
	@assert(v != "123")
}

//#[swag.printbc]
#test
{
	var t: titi
	t.buf[0] = acast '1'
	t.buf[1] = acast '2'
	t.buf[2] = acast '3'
	t.buf[3] = 0
	
	var x = t.toString()
	toto(x)
}