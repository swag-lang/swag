struct titi
{
	buf: [5] u8;
}

//#[swag.printbc]
func toto(xx: titi, x: s64)
{
    @assert(x == 666)
}

//#[swag.printbc]
#test
{
	var t: titi
	toto(t, 666)
}