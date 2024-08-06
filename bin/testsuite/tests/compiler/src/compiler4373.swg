#global testerror "Err0601"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A) add() where isInteger(A)
{
}

#test
{
    add's32()
}