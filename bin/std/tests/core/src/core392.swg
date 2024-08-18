using Core

#test
{
    var h: HashTable'(s32, String)
    loop i in 50:
        h.add(i, String.from("s"))
}
