//#[Swag.PrintBc]
func(T) sum(x, y: *T) => dref x + dref y
#test
{
    var a = 1
    var b = 2
    @assert(sum(&a, &b) == 3)

    b = 10
    @assert(sum(&a, &b) == 11)

    var c = 1.0
    var d = 5.5
    @assert(sum(&c, &d) == 6.5)
}
