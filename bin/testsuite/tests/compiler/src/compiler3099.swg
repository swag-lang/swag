//#global testerror

#[Swag.ConstExpr]
func msg() => "msg"

#test
{
    x := 1
    #assert true, msg()
}