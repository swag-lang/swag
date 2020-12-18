
#unittest error

namespace A
{
    func isZeroEpsilon(x: bool) => false
}

func isZeroEpsilon(x: f32) => false
func isZeroEpsilo1(x: f32) => false
func isZeroEpsilo2(x: f32) => false
func isZeroEpsilo3(x: f32) => false
func isZeroEpsilo4(x: f32) => false

using A

#test
{
    isZeroEpsilon(true)
}
