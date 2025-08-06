using Core

//#[Swag.PrintBc]
#test
{
    var h: HashTable'(GUID, string)

    var v1 = GUID.getRandom()
    var v2 = GUID.getRandom()

    h.add(v1, "toto")
    h.add(v2, "titi")
    @assert(h.contains(v1))
    @assert(h.contains(v2))
}
