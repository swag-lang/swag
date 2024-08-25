using Core

#test
{
    var h: HashTable'(s32, String)
    for i in 50:
        h.add(i, String.from("s"))

    foreach k, v in h
    {
        @assert(v[0] == `s`'u8)
    }
}
