#global testerror "Err0611"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A) add() where isInteger(A)
{
}

#test
{
    add's32()
}