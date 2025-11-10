//#[Swag.PrintBc]
#test
{
    var c = [1, 2, 3]
    let d = @dataof(c)
    @assert(d[0] == 1)
    @assert(d[1] == 2)
    @assert(d[2] == 3)
}
