/*#[Swag.ConstExpr, Swag.Macro]
func toto(T: s32)->s32
{
    #ast
    {
        var az: [512] u8
        @memcpy(&az[0], @dataof("return "), 7)
        for i in T
            az[i + 7] = '1''u8

        var str1 = @mkstring(&az[0], 16)
        return str1
    }
}

const str = toto(10)
#assert(str == 111111111*/

#[Swag.Inline]
func toto(t: typeinfo) => t.fullname

//#[Swag.PrintBc]
#test
{
    var s = #typeof(bool)
    let x = toto(s32)
    @assert(x == "s32")
}
