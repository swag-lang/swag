using Core

#test
{
    var h: HashTable'(s32, String)
    loop i: 50
    {
        x := String.from("s")
        h.add(i, moveref &x)
        @assert(x.length == 0)
    }
}