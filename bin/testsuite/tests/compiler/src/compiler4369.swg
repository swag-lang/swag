#global testerror "Err0085"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A) add() #validif isInteger(A)
{
}

#test
{
    add's32()
}