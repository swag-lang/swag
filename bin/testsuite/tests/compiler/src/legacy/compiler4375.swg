#global testerror "Err0078"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A) add(x: A)
    where isInteger(A)
{
}

#test
{
    add(0's32)
}
