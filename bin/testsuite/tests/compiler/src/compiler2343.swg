#[Swag.ConstExpr, Swag.Macro]
func(T: s32) toto()->s32
{
    #ast
    {
        var az: [512] u8
        @memcpy(&az[0], @dataof("return "), 7)
        for i in T:
            az[i + 7] = '1''u8

        let str1 = @mkstring(&az[0], 16)
        return str1
    }
}

const str = toto'10()
#assert(str == 111111111)
