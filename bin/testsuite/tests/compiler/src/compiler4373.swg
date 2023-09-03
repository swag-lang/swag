#global testerror "Err0678"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A: isInteger(A)) add()
{
}

#test
{
    add's32()
}