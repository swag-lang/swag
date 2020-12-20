#[swag.constexpr, swag.macro]
func toto(rep: s32)->s32
{
    #ast
    {
        var az: [512] u8
        @memcpy(&az[0], @dataof("return "), 7)
        loop i: rep
            az[i + 7] = "1"'u8

        str1 := cast(string) @mkslice(&az[0], 16)
        return str1
    }
}

const str = toto(10)
#assert str == 111111111
