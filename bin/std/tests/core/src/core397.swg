using Core

//#[Swag.PrintBc]
#test
{
    var h: HashTable'(s32, String)
    loop i: 50 do
        h.add(i, String.from("s"))

    var h1 = h

    visit k, v: h do
        @assert(v[0] == `s`'u8)
    visit k, v: h1 do
        @assert(v[0] == `s`'u8)
}