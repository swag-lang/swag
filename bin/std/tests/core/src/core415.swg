using Core

#test
{
    var a: StaticArray'(String, 50)
    a.add(String.from("a"))
    a.add(String.from("b"))

    b := a
    //@assert(b.contains(String.from("a")))
}