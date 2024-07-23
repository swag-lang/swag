//#global testerror

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false
#[Swag.ConstExpr]
func isFloat(type: typeinfo) => true

func(var A) add() where isInteger(A) or isFloat(A)
{
}

#test
{
    add's32()
}