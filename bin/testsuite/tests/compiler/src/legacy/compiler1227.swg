#[Swag.Overload]
func isZeroEpsilon(x: f32) => false
#[Swag.Overload]
func isZeroEpsilon() => isZeroEpsilon(0)
