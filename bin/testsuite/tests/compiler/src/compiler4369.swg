#global testerror "Err0060"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A) add() where isInteger(A)
{
}

#test
{
    add's32()
}