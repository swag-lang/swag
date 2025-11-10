//#[Swag.PrintBc]
//#[Swag.PrintAsm]
#test
{
    var a    = 10
    var c    = 20
    var res  = {&a, &c}
    let res1 = res
    @assert(dref res1.item0 == 10)
    @assert(dref res1.item1 == 20)
}
