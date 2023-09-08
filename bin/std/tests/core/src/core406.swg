//#global skip
using Core, StrConv

#[Swag.Macro]
func(str: string) interpol()->String
{
    var sb: StringBuilder

    #ast
    {
        var d = @dataof(str)
        while dref d != `{`'u8
            d += 1
        d += 1

        var sb1: StringBuilder
        sb1.appendFormat("sb.appendAny(#up %)", @mkstring(d, 1))
        return sb1.toString()
    }

    return sb.toString()
}

#test
{
    let a = 666
    let b = 52
    let x = interpol'"{a}"()
    Debug.assert(x == "666")
    let y = interpol'"{b}"()
    Debug.assert(y == "52")
}