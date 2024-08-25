using Core

struct ST
{
    str: String
}

//#[Swag.PrintBc]
#test
{
    var result: Array'ST
    var x:      ST
    var y:      [8] u8
    y[0] = `A`'u8
    y[1] = `B`'u8
    y[2] = 0
    let dataof = cast(const ^u8) y

    for 20
    {
        x.str = @mkstring(dataof, 2)
        result.add(x)
    }

    visit &c in result:
        @assert(c.str == "AB")
}
