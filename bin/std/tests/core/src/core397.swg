using Core

//#[Swag.PrintBc]
#test
{
    var h: HashTable'(s32, String)
    for i in 50:
        h.add(i, String.from("s"))

    var h1 = h

    foreach k, v in h:
        @assert(v[0] == `s`'u8)
    foreach k, v in h1:
        @assert(v[0] == `s`'u8)
}
