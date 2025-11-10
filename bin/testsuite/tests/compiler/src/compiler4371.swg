//#global testerror

//#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A) add()
    where #run isInteger(A)
{
}

#test
{
    add's32()
}
