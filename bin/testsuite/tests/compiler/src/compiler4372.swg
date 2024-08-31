//#global testerror "Err0294"

//#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A) add() where isInteger(A)
{
}

#test
{
    add's32()
}