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

//#[swag.printbc]
func toto(v: string)
{
	@assert(v == "1234")
}

#test
{
	var t: titi
	t.buf[0] = autocast "1"'rune
	t.buf[1] = autocast "2"'rune
	t.buf[2] = autocast "3"'rune
	t.buf[3] = autocast "4"'rune

	var x = t.toString()
	toto(cast(string) x)
}