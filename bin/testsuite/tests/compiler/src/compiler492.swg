//#[Swag.PrintBc]
#test
{
    var a   = 10
    var c   = 20
    var res = {&a, &c}
    @assert(dref res.item0 == 10)
    @assert(dref res.item1 == 20)
}
