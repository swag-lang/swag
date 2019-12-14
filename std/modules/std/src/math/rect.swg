namespace std.math
{
    /// A simple rectangle  with 4 coordinates
	public struct Rect
	{
        x:      f32
        y:      f32
        width:  f32
        height: f32
	}

	impl Rect
	{
        public
        {
            #[swag.complete]
			func opAffect(using self, value: f32)
			{
				x, y, width, height = value
			}

            func opEquals(using self, other: Self)->bool
            {
                return x == other.x && y == other.y && width == other.width && height == other.height
            }
        }

		public
		{
            public
    		{
    			const Zero: Rect(0, 0, 0, 0)
            }

            /// Set point to (0, 0)
			func clear(using self)
			{
				x, y, width, height = 0
			}

            /// Returns true if this instance is null
            func isZero(using self)->bool
            {
                return x == 0 && y == 0 && width == 0 && height == 0
            }

            /// Returns true if this instance is null with an epsilon
            func isZeroEpsilon(using self, eps: f32 = constF32.Epsilon)->bool
            {
                return isZeroEpsilon(x, eps) && isZeroEpsilon(y, eps) && isZeroEpsilon(width, eps) && isZeroEpsilon(height, eps)
            }

            /// Offset this point by a given value
            func offset(using self, value: f32)
            {
                x, y += value
            }

            /// Multiply the width and height of the rectangle by a given value
            func scale(using self, value: f32)
            {
                width, height *= value
            }

            /// Multiply the width and height of the rectangle by a given value
            func scale(using self, valueX, valueY: f32)
            {
                width *= valueX
                height *= valueY
            }
		}
	}
}
