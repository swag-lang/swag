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

            func opEquals(using self, other: Self)->bool
            {
                return x == other.x && y == other.y
            }

            func(op: string) opAssign(using self, other: Self)
            {
                #if   op == "+="    { x += other.x; y += other.y; }
                #elif op == "-="    { x -= other.x; y -= other.y; }
                #elif op == "*="    { x *= other.x; y *= other.y; }
                #else               #assert(false, "unsupported assign operation")
            }
            
            func(op: string) opAssign(using self, value: Float)
            {
                #if   op == "+="    { x += value; y += value; }
                #elif op == "-="    { x -= value; y -= value; }
                #elif op == "*="    { x *= value; y *= value; }
                #else               #assert(false, "unsupported assign operation")
            }
            
            func(op: string) opBinary(using self, other: Self)->Self
            {
                #if   op == "+" return Vector2(x + other.x, y + other.y)
                #elif op == "-" return Vector2(x - other.x, y - other.y)
                #elif op == "*" return Vector2(x * other.x, y * other.y)
                #else           #assert(false, "unsupported binary operation")
            }

            func(op: string) opBinary(using self, value: Float)->Self
            {
                #if   op == "+" return Vector2(x + value, y + value)
                #elif op == "-" return Vector2(x - value, y - value)
                #elif op == "*" return Vector2(x * value, y * value)                    
                #else           #assert(false, "unsupported binary operation")
            }
        }

		public
		{
			func length(using self) => sqrt((x * x) + (y * y))
			func lengthSquared(using self) => (x * x) + (y * y)
			func dot(using self, other: Self) => (x * other.x) + (y * other.y)

            func clear(using self)
            {
                x = 0
                y = 0
            }

            func negate(using self)
            {
                x = -x
                y = -y
            }

            func isZero(using self)->bool
            {
                return x == 0 && y == 0
            }

            func isZeroEpsilon(using self, eps: Float = cast(Float) constF64.Epsilon)->bool
            {
                return isZeroEpsilon(x, eps) && isZeroEpsilon(y, eps)
            }

            func distance(using self, to: Self)->Float
            {
                xn := (x - to.x)
                yn := (y - to.y)
                return sqrt((xn * xn) + (yn * yn))
            }

            func distanceSquared(using self, to: Self)->Float
            {
                xn := (x - to.x)
                yn := (y - to.y)
                return (xn * xn) + (yn * yn)
            }

            func normalize(using self)
            {
                norm := sqrt((x * x) + (y * y))
                diagnostics.debug.assert(!isZeroEpsilon(norm))
                norm = 1 / norm
                x *= norm
                y *= norm
            }

            func normalized(using self)->Vector2
            {
                norm := sqrt((x * x) + (y * y))
                diagnostics.debug.assert(!isZeroEpsilon(norm))
                norm = 1 / norm
                return Vector2(x * norm, y * norm)
            }

            func normalizeSafe(using self)
            {
                norm := sqrt((x * x) + (y * y))
                if isZeroEpsilon(norm)
                {
                    x = 0
                    y = 0
                    return
                }

                norm = 1 / norm
                x *= norm
                y *= norm
            }

            func normalizedSafe(using self)->Vector2
            {
                norm := sqrt((x * x) + (y * y))
                if isZeroEpsilon(norm)
                    return Zero
                norm = 1 / norm
                return Vector2(x * norm, y * norm)
            }
		}
	}
}
