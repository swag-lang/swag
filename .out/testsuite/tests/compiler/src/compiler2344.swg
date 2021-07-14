/*#[Swag.ConstExpr, Swag.macro]
func toto(T: s32)->s32
{
    #ast
    {
        var az: [512] u8
        @memcpy(&az[0], @dataof("return "), 7)
        loop i: T
            az[i + 7] = "1"'u8

        str1 := @mkstring(&az[0], 16)
        return str1
    }
}

const str = toto(10)
#assert str == 111111111*/

#[Swag.inline]
func toto(t: typeinfo) => t.name

//#[Swag.PrintBc]
#test
{
    s := @typeof(bool)
    x := toto(s32)
    @assert(x == "s32")
}
