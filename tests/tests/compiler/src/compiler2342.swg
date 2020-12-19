/*#[swag.constexpr, swag.macro]
func toto(val: u8, cpt: u32)->string
{
    #[swag.printbc]
    #ast
    {
        var az: [512] u8
        loop i: 10'uint az[i] = "X"'u8
        return cast(string) @mkslice(&az[0], 10)
    }
}

const str = toto("X"'u8, 10)
*/