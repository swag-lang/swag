//#[swag.printbc]
#test
{
    a, b, c := 5
    @assert(a == b and b == c)
    @assert(a == b and b == c and c == 5)
    @assert(1 < b and b < 10)

    a = 5
    b = 15
    @assert(0 < a and a < 10 and 10 < b and b < 20)
    @assert(5 == a and a != b)
}