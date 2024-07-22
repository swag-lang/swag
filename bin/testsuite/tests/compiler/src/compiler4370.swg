//#global testerror

#[Swag.ConstExpr]
func isInteger(type: typeinfo) => true

func(var A where isInteger(A)) add()
{
}

#test
{
    add's32()
}