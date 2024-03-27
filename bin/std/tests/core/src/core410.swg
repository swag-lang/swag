using Core

//#[Swag.PrintBc]
#test
{
    var h: HashTable'(UUID, string)

    var v1 = UUID.getRandom()
    var v2 = UUID.getRandom()

    h.add(v1, "toto")
    h.add(v2, "titi")
    @assert(h.contains(v1))
    @assert(h.contains(v2))
}