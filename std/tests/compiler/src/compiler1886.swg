//#[swag.printbc]
#test
{
    a, b, c := 5
    @assert(a == b == c)
    @assert(a == b == c == 5)
    @assert(1 < b < 10)

    a = 5
    b = 15
    @assert(0 < a < 10 < b < 20)
    @assert(5 == a != b)
}