//#global testerror

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false
#[Swag.ConstExpr]
func isFloat(type: typeinfo) => true

func(var A: isInteger(A) or isFloat(A)) add()
{
}

#test
{
    add's32()
}