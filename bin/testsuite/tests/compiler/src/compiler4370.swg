//#global testerror

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A) add() #validif isInteger(A)
{
}

#test
{
    add's32()
}