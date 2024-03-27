#[Swag.ConstExpr]
func abs(x: f32) => -x

//#[Swag.PrintBc]
#test
{
    let y = -1'f32
    let z = abs(y)
    @assert(z == 1)
}
