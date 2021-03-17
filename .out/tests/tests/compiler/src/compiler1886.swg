//#[swag.printbc]
#test
{
    a, b, c := 5
    @assert(a == b && b == c)
    @assert(a == b && b == c && c == 5)
    @assert(1 < b && b < 10)

    a = 5
    b = 15
    @assert(0 < a && a < 10 && 10 < b && b < 20)
    @assert(5 == a && a != b)
}