#global testerror "Err0540"
struct A
{
    x: string
}

impl A
{
    func opData(using self)->const *void
    {
        return @dataof(x)
    }
}

//#[Swag.PrintBc]
#test
{
    var v1: A
    var v2: A

    v2.x = "value2"

    var v3 = v1 orelse v2
    //@assert(v3.x == "value2")

    v1.x = "value1"
    v3 = v1 orelse v2
    //@assert(v3.x == "value1")
}