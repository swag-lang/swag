#global testerror "Err0083"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A) add() where isInteger(A)
{
}

#test
{
    add's32()
}