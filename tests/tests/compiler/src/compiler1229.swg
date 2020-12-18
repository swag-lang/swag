namespace std.math
{
    alias Float = f32
    func isZeroEpsilon(x, y: Float) => true
}

namespace std.math
{
	struct Vector2
	{
		x, y: Float
	}

	impl Vector2
	{
		func isZeroEpsilon(using self, eps: Float = cast(Float) 1)->bool
		{
			return isZeroEpsilon(x, eps)
		}
	}
}