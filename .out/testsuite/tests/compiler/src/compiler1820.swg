struct titi
{
	buf: [5] u8;
}

//#[Swag.PrintBc]
impl titi
{
    func toto(xx: titi, x: s64)
    {
        @assert(x == 666)
    }
}

//#[Swag.PrintBc]
#test
{
    arr := @[10, 20, 320]
	var t: titi
	titi.toto(t, 666)
}