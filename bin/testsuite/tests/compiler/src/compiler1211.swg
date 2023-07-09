#[Swag.ConstExpr]
func abs(x: f32) => -x

//#[Swag.PrintBc]
#test
{
    var y = -1'f32
    var z = abs(y)
    @assert(z == 1)
}