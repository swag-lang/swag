using Core

//#[Swag.PrintBc]
#test
{
    var arr: HashTable'(String, s64)
    arr.clear()

    var x = String.from("toto")
    var y = String.from("toto")
    @assert(x == y)
    y = "titi"
    @assert(x != y)
}
