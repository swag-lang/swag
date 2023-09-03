#global testerror "Err0118"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A: isInteger(A)) add()
{
}

#test
{
    add's32()
}