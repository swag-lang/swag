#global testerror "Err0128"

//#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A: isInteger(A)) add()
{
}

#test
{
    add's32()
}