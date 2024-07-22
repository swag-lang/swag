#global testerror "Err0398"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A where isInteger(A)) add()
{
}

#test
{
    add's32()
}