struct titi
{
    buf: [4] u8
}

impl titi
{
    func toString(me)->const [..] u8
    {
        return @mkslice(&me.buf[0], 4)
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
    t.buf[0] = cast() '1''rune
    t.buf[1] = cast() '2''rune
    t.buf[2] = cast() '3''rune
    t.buf[3] = 0

    let x = t.toString()
    toto(cast(string) x)
}
