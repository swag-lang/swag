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
			const Null:  Vector2(0, 0)
			const Unit:  Vector2(1, 1)
			const UnitX: Vector2(1, 0)
			const UnitY: Vector2(0, 1)
		}
	}
}