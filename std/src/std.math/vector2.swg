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
			const Zero:     Vector2(0, 0)
			const One:      Vector2(1, 1)
			const UnitX:    Vector2(1, 0)
			const UnitY:    Vector2(0, 1)
			const UnitNX:   Vector2(-1, 0)
			const UnitNY:   Vector2(0, -1)		
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
            
            func clear(using self)
            {
                x = 0
                y = 0
            }
            
            func isZero(using self)->bool
            {
                return x == 0 && y == 0
            }
                 
            func isZeroEpsilon(using self, eps: Float = cast(Float) constF64.Epsilon)->bool
            {
                return isZeroEpsilon(x, eps) && isZeroEpsilon(y, eps)
            }
               
            func makeNormalize(using self)
            {
                norm := sqrt((x * x) + (y * y))
                diagnostics.debug.assert(!isZeroEpsilon(norm))                
                norm = 1 / norm
                x *= norm
                y *= norm
            }
            
            func normalize(using self)->Vector2
            {
                norm := sqrt((x * x) + (y * y))
                diagnostics.debug.assert(!isZeroEpsilon(norm))                
                norm = 1 / norm
                return Vector2(x * norm, y * norm)
            }            
		}
	}
}