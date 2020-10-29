public namespace std.math;

// A simple rectangle with 4 coordinates
struct Rect
{
    x:      f32
    y:      f32
    width:  f32
    height: f32
}

impl Rect
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

    #[swag.property, swag.inline]
    {
        // right coordinate (x + width)
        func right(using self)->f32
        {
            return x + width
        }

        // right coordinate (x + width)
        func right(using self, value: f32)
        {
            width = value - x
        }

        // bottom coordinate (y + height)
        func bottom(using self)->f32
        {
            return y + height
        }

        // bottom coordinate (y + height)
        func bottom(using self, value: f32)
        {
            height = value - y
        }
    }

    // Represents a rectangle with all coordinates to zero
    const Zero: Rect{0, 0, 0, 0}

    // Set rectangle to (0, 0, 0, 0)
    func clear(using self)
    {
        x, y, width, height = 0
    }

    // Returns true if this instance is null
    func isZero(using self)->bool
    {
        return x == y == width == height == 0
    }

    // Returns true if this instance has a surface of zero (width and heigth are null)
    func isEmpty(using self)->bool
    {
        return width == height == 0
    }

    // Returns true if this instance has a positive surface (width and heigth greater than zero)
    func isNormalized(using self)->bool
    {
        return width >= 0 && height >= 0
    }

    // Offset this point by a given value
    func offset(using self, value: f32)
    {
        x, y += value
    }

    // Multiply the width and height of the rectangle by a given value
    func scale(using self, value: f32)
    {
        width, height *= value
    }

    // Multiply the width and height of the rectangle by a given value
    func scale(using self, valueX, valueY: f32)
    {
        width *= valueX
        height *= valueY
    }

    // Returns true if this instance is null with an epsilon
    func isZeroEpsilon(using self, eps = ConstF32.Epsilon)->bool
    {
        return math.isZeroEpsilon(x, eps) && math.isZeroEpsilon(y, eps) && math.isZeroEpsilon(width, eps) && math.isZeroEpsilon(height, eps)
    }

    // Returns true if this instance has a surface of zero (width and heigth are null)
    func isEmptyEpsilon(using self, eps = ConstF32.Epsilon)->bool
    {
        return math.isZeroEpsilon(width, eps) && math.isZeroEpsilon(height, eps)
    }

    // Transform a RectF to a Rect
    func round(using self)->Rect
    {
        x = acast math.round(x)
        y = acast math.round(y)
        width = acast math.round(width)
        height = acast math.round(height)
    }

    // Transform a RectF to a Rect
    func trunc(using self)
    {
        x = acast math.trunc(x)
        y = acast math.trunc(y)
        width = acast math.trunc(width)
        height = acast math.trunc(height)
    }

    // Determines if the specfied point is contained within the rectangular region
    func contains(self, x, y: f32)->bool
    {
        return (self.x >= x < self.x + self.width) && (self.y >= y < self.y + self.height)
    }

    // Determines if the specfied point is contained within the rectangular region
    func contains(using self, pt: Point)->bool
    {
        return self.contains(pt.x, pt.y)
    }
}
