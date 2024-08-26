//#global testerror "Err0293"

//#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A) add() where isInteger(A)
{
}

#test
{
    add's32()
}