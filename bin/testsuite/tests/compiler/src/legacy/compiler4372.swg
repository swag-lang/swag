//#global testerror "Err0168"

//#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A) add()
    where isInteger(A)
{
}

#test
{
    add's32()
}
