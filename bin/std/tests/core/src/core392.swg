using Core

#test
{
    var h: HashTable'(s32, String)
    for i in 50 do
        h.add(i, String.from("s"))
}
