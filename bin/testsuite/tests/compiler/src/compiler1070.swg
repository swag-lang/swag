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

//#[Swag.PrintBc]
#test
{
	var t: titi
	t.buf[0] = acast "1"'rune
	t.buf[1] = acast "2"'rune
	t.buf[2] = acast "3"'rune
	t.buf[3] = 0

	var x = t.toString()
	toto(cast(string) x)
}