using Core

#test
{
    var h: HashTable'(s32, String)
    loop i: 50 do
        h.add(i, String.from("s"))

    visit k, v: h
    {
        @assert(v[0] == `s`'u8)
    }
}