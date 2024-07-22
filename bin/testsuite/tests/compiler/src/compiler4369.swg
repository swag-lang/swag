#global testerror "Err0088"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A where isInteger(A)) add()
{
}

#test
{
    add's32()
}