
using Core

#[Swag.ConstExpr]
func toto(rep: u32)->String
{
    var result: retval
    loop rep result += "X"
    return result
}

const str = cast(string) toto(10)
#assert str == "XXXXXXXXXX"