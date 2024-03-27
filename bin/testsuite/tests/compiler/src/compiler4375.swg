#global testerror "Err0088"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A: isInteger(A)) add(x: A)
{
}

#test
{
    add(0's32)
}