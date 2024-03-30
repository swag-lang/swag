using Core

#test
{
    var a: StaticArray'(String, 50)
    a.add(String.from("a"))
    a.add(String.from("b"))

    var b = a
    @assert(b.contains(String.from("a")))
    @assert(b.contains(String.from("b")))
    @assert(b.contains("a"))
    @assert(b.contains("b"))
}
