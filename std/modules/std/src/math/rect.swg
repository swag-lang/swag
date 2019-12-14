namespace std.math
{
    /// A simple rectangle  with 4 coordinates
	public struct Rect
	{
		left:   f32
        top:    f32
        right:  f32
        bottom: f32
	}

	impl Rect
	{
        public
        {
            #[swag.complete]
			func opAffect(using self, value: f32)
			{
				left, top, right, bottom = value
			}

            func opEquals(using self, other: Self)->bool
            {
                return left == other.left && top == other.top && right == other.right && bottom == other.bottom
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
				left, top, right, bottom = 0
			}

            /// Returns true if this instance is null
            func isZero(using self)->bool
            {
                return left == 0 && top == 0 && right == 0 && bottom == 0
            }

            /// Returns true if this instance is null with an epsilon
            func isZeroEpsilon(using self, eps: f32 = cast(f32) constF64.Epsilon)->bool
            {
                return isZeroEpsilon(left, eps) && isZeroEpsilon(top, eps) && isZeroEpsilon(right, eps) && isZeroEpsilon(bottom, eps)
            }

            /// Offset this point by a given value
            func offset(using self, value: f32)
            {
                left, top += value
            }
		}
	}
}
