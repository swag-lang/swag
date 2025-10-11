#[Swag.ConstExpr, Swag.Macro]
func toto()->s32
{
    #ast
    {
        var az: [512] u8
        @memcpy(&az[0], @dataof("return "), 7)
        for i in 10'u64 do
            az[i + 7] = '1''u8

        let str1 = @mkstring(&az[0], 16)
        return str1
    }
}

const str = toto()
#assert(str == 111111111)
