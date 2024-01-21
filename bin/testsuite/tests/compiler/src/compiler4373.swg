#global testerror "Err0399"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A: isInteger(A)) add()
{
}

#test
{
    add's32()
}