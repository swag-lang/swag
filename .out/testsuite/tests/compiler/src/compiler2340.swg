#[Swag.ConstExpr, Swag.Macro]
func toto1()->u32 => 666
const str1 = toto1()
#assert str1 == 666