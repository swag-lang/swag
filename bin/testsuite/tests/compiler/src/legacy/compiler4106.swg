struct Entry
{
    length:      u16
    length1:     u16
    length2:     u16
}

//#[Swag.PrintBc]
func toto(x: Entry)
{
    @assert(x.length == 0x10)
    @assert(x.length1 == 0x20)
    @assert(x.length2 == 0x40)
}

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, false), Swag.Optimize(.All, false)]
#test
{
    var key = 66
    toto({0x10, 0x20, 0x40})
}
