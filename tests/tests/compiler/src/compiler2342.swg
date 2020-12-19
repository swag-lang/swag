#[swag.constexpr, swag.macro]
func toto(val: u8, cpt: u32)->string
{
    #ast
    {
        var az: [512] u8
        loop i: cpt az[i] = val
        return cast(string) @mkslice(&az[0], cpt)
    }
}

const str = toto("X"'u8, 10)

#test
{
    @print(str)
}
