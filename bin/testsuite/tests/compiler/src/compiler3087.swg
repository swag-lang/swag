//#[Swag.PrintBc]
#test
{
    let x = 6
    let y = 8
    var g = 0
    for i in x until y:
        g += i
    @assert(g == 6 + 7)
}