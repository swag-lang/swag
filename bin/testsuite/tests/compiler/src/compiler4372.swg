#global testerror "Err0044"

//#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A where isInteger(A)) add()
{
}

#test
{
    add's32()
}