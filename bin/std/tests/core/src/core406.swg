//#global skip
using Core, StrConv

#[Swag.Macro]
func(str: string) interpol()->String
{
    var sb: StringBuilder

    #ast
    {
        d := @dataof(str)
        while dref d != "{"'u8
            d += 1
        d += 1

        var sb1: StringBuilder
        sb1.appendFormat("sb.appendAny(`%)", @mkstring(d, 1))
        return sb1.toString()
    }

    return sb.toString()
}

#test
{
    a := 666
    b := 52
    x := interpol'"{a}"()
    Debug.assert(x == "666")
    y := interpol'"{b}"()
    Debug.assert(y == "52")
}