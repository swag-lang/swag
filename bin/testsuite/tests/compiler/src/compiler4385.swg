//#global testerror

#[Swag.ConstExpr, Swag.Mixin]
func isF32(t: typeinfo)->bool => t == f32

//#global testerror
#test
{
    var x = isF32(f32)
}
