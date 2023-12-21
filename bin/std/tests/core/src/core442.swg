using Core

#test
{
    struct Key
    {
        r: Parser.RegExp
        b: u32
    }

    var byr_r: Parser.RegExp
    assume byr_r.compile("abcd")

    var map: HashTable'(string, Key)
    map.add("a", {byr_r, 1})
    map.add("b", {byr_r, 1})
    map.add("c", {byr_r, 1})
    map.add("d", {byr_r, 1})
    map.add("e", {byr_r, 1})
    map.add("f", {byr_r, 1})
    map.add("g", {byr_r, 1})
    map.add("h", {byr_r, 1})

    let it = map.find("a")
    @assert(it != null)
    @assert(it.value.r.match("abcd"))
}