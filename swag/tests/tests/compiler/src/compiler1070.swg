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

func toto(v: string)
{
	@assert(v != "123")
}

//#[swag.printbc]
#test
{
	var t: titi
	t.buf[0] = autocast "1"'char
	t.buf[1] = autocast "2"'char
	t.buf[2] = autocast "3"'char
	t.buf[3] = 0

	var x = t.toString()
	toto(cast(string) x)
}