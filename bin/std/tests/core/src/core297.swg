using Core

#test
{
    var h: HashSet'String
    h.add(String.from("toto"))
    h.add(String.from("titi"))
    @assert(h.contains(String.from("titi")))
    @assert(h.contains(String.from("toto")))
    @assert(!h.contains(String.from("tata")))

    let h1 = h
    @assert(h1.contains(String.from("titi")))
    @assert(h1.contains(String.from("toto")))
    @assert(!h1.contains(String.from("tata")))
}
