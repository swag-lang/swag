using Core

#test
{
    var h: HashTable'(s32, String)
    loop i: 50
    {
        x := String.from("s")
        h.emplace(i, &x)
        @assert(x.length == 0)
    }
}