#global public
#global namespace math

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
    // Represents a rectangle with all coordinates to zero
    const Zero: Rect{0, 0, 0, 0}

    func opEquals(using self, other: Rect)->bool
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

    // Returns true if this instance has a surface of zero (width and height are null)
    func isEmpty(using self)->bool
    {
        return width == height == 0
    }

    // Returns true if this instance has a positive surface (width and height greater than zero)
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
    func scale(self, x, y: f32)
    {
        self.width *= x
        self.height *= y
    }

    // Inflates the rectangle by the given amount
    func inflate(self, x, y: f32)
    {
        self.x -= x
        self.y -= y
        self.width += x * 2
        self.height += x * 2
    }

    // Returns true if this instance is null with an epsilon
    func isZeroEpsilon(using self, eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) && isZeroEpsilon(y, eps) && isZeroEpsilon(width, eps) && isZeroEpsilon(height, eps)
    }

    // Returns true if this instance has a surface of zero (width and height are null)
    func isEmptyEpsilon(using self, eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(width, eps) && isZeroEpsilon(height, eps)
    }

    // Perform a math.round operation on all the coordinates
    func round(using self)
    {
        x = autocast round(x)
        y = autocast round(y)
        width = autocast round(width)
        height = autocast round(height)
    }

    // Perform a math.trunc operation on all the coordinates
    func trunc(using self)
    {
        x = autocast trunc(x)
        y = autocast trunc(y)
        width = autocast trunc(width)
        height = autocast trunc(height)
    }

    // Perform a math.ceil operation on all the coordinates
    func ceil(using self)
    {
        x = autocast ceil(x)
        y = autocast ceil(y)
        width = autocast ceil(width)
        height = autocast ceil(height)
    }

    // Determines if the specfied point is contained within the rectangular region
    func contains(self, x, y: f32)->bool
    {
        return (self.x >= x < self.x + self.width) && (self.y >= y < self.y + self.height)
    }

    // Determines if the specfied point is contained within the rectangular region
    func contains(using self, pt: Point)->bool
    {
        return contains(pt.x, pt.y)
    }

    // Determines if the specfied rectangle is contained within the rectangular region
    func contains(using self, rect: Rect)->bool
    {
        return (rect.x >= x && rect.right <= right) && (rect.y >= y && rect.bottom <= bottom)
    }

    // Intersect this rectangle with another one
    func intersect(using self, with: Rect)
    {
        :self = intersect(:self, with)
    }

    // Creates a rectangle that represents the intersetion between 'a' and 'b'.
    // If there is no intersection, Zero is returned.
    func intersect(a, b: Rect)->Rect
    {
        x1 := max(a.x, b.x)
        x2 := min(a.x + a.width, b.x + b.width)
        y1 := max(a.y, b.y)
        y2 := min(a.y + a.height, b.y + b.height)
        if x2 >= x1 && y2 >= y1
            return Rect{x1, y1, x2 - x1, y2 - y1}
        return Zero;
    }

    // Determines if this rectangle intersets with 'rect'
    func intersectWith(using self, rect: Rect)->bool
    {
        return (rect.x < right) && (x < rect.right) && (rect.y < bottom) && (y < rect.bottom)
    }

    // Creates a rectangle that represents the union
    func setUnion(using self, with: Rect)
    {
        :self = getUnion(:self, with)
    }

    // Creates a rectangle that represents the union between 'a' and 'b'.
    func getUnion(a, b: Rect)->Rect
    {
        x1 := min(a.x, b.x)
        x2 := max(a.right, b.right)
        y1 := min(a.y, b.y)
        y2 := max(a.bottom, b.bottom)
        return Rect{x1, y1, x2 - x1, y2 - y1}
    }
}
