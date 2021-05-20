
using core

//#[Swag.printbc]
#test
{
    var arr: HashTable'(String, s64)
    arr.clear()

    x := String.from("toto")
    y := String.from("toto")
    @assert(x == y)
    y = "titi"
    @assert(x != y)
}