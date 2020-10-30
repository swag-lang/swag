/*struct A
{

}

impl A
{
    func toto(using self, x: A)
    {
        @print("v1\n")
    }

    func toto(x, y: A)->A
    {
        @print("v2\n")
        var v: A
        return v
    }
}

#test
{
    var a, b: A
    c := A.toto(a, b)
}*/