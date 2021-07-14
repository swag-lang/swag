#[Swag.ConstExpr]
func abs(x: f32) => -x

//#[Swag.PrintBc]
#test
{
    y := -1'f32
    z := abs(y)
    @assert(z == 1)
}