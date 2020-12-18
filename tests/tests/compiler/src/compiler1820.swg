struct titi
{
	buf: [5] u8;
}

//#[swag.printbc]
impl titi
{
    func toto(xx: titi, x: s64)
    {
        @assert(x == 666)
    }
}

//#[swag.printbc]
#test
{
    arr := @[10, 20, 320]
	var t: titi
	titi.toto(t, 666)
}