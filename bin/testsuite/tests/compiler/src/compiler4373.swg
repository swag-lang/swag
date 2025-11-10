#global testerror "Err0633"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => 1

func(var A) add()
    where isInteger(A)
{
}

#test
{
    add's32()
}
