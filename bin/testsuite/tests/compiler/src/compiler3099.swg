//#global testerror

#[Swag.ConstExpr]
func msg() => "msg"

#test
{
    var x = 1
    #if false: #error(msg())
}