#global testerror "Err0550"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A) add() where isInteger(A)
{
}

#test
{
    add's32()
}