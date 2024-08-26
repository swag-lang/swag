#global testerror "Err0080"

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => false

func(var A) add(x: A) where isInteger(A)
{
}

#test
{
    add(0's32)
}