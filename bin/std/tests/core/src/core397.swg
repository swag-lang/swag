using Core

//#[Swag.PrintBc]
#test
{
    var h: HashTable'(s32, String)
    for i in 50 do
        h.add(i, String.from("s"))

    let h1 = h

    foreach k, v in h do
        @assert(v[0] == 's''u8)
    foreach k, v in h1 do
        @assert(v[0] == 's''u8)
}
