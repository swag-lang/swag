namespace std.math
{
    typealias Float: f32
    func isZeroEpsilon(x, y: Float) => true
}

namespace std.math
{
	public struct Vector2
	{
		x, y: Float
	}
	
	impl Vector2
	{
		public
		{
            func isZeroEpsilon(using self, eps: Float = cast(Float) 1)->bool
            {
                return isZeroEpsilon(x, eps)
            }         
		}
	}
}