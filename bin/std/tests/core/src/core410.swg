using Core

//#[Swag.PrintBc]
#test
{
    var h: HashTable'(GUID128, string)

    let v1 = GUID128.getRandom()
    let v2 = GUID128.getRandom()

    h.add(v1, "toto")
    h.add(v2, "titi")
    @assert(h.contains(v1))
    @assert(h.contains(v2))
}
