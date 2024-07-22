#global testerror "Err0191"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A) add() #validif isInteger(A)
{
}

#test
{
    add's32()
}