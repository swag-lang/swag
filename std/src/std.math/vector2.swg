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
			const Zero:  Vector2(0, 0)
			const One:   Vector2(1, 1)
			const UnitX: Vector2(1, 0)
			const UnitY: Vector2(0, 1)
		}
		
		public
		{
			#[swag.complete]
			func opAffect(self, value: Float)
			{
				self.x = value
				self.y = value
			}
			
			func length(using self) => sqrt((x * x) + (y * y))
			func lengthSquared(using self) => (x * x) + (y * y)
			func dot(using self, other: Self) => (x * other.x) + (y * other.y)
		}
	}
}