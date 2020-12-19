#[swag.constexpr, swag.macro]
func toto1(cpt: u32)->u32 => cpt
const str1 = toto1(666)
#assert str1 == 666