namespace A
{
    func isZeroEpsilon(x: bool) => false
}

func isZeroEpsilon(x: f32) => false
using A

#test
{
    isZeroEpsilon(0.0)
}
