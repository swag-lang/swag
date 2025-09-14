using Core, Core.Utf8

//#[Swag.PrintBc]
#test
{
    let x: String = "ABCDE昨"
    @assert(x.firstRune() == 'A')
    @assert(x.lastRune() == '昨')
}
