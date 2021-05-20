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

    func opEquals(using const self, other: Rect)->bool
    {
        return x == other.x and y == other.y and width == other.width and height == other.height
    }

    // right coordinate (x + width)
    #[Swag.inline]
    func right(using const self)->f32
    {
        return x + width
    }

    // right coordinate (x + width)
    #[Swag.inline]
    func setRight(using self, value: f32)
    {
        width = value - x
    }

    // bottom coordinate (y + height)
    #[Swag.inline]
    func bottom(using const self)->f32
    {
        return y + height
    }

    // bottom coordinate (y + height)
    #[Swag.inline]
    func setBottom(using self, value: f32)
    {
        height = value - y
    }

    // Set rectangle to (0, 0, 0, 0)
    func clear(using self)
    {
        x, y, width, height = 0
    }

    // Returns true if this instance is null
    func isZero(using const self)->bool
    {
        return x == 0 and y == 0 and width == 0 and height == 0
    }

    // Returns true if this instance has a surface of zero (width and height are null)
    func isEmpty(using const self)->bool
    {
        return width == 0 and height == 0
    }

    // Returns true if this instance has a positive surface (width and height greater than zero)
    func isNormalized(using const self)->bool
    {
        return width >= 0 and height >= 0
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
    func isZeroEpsilon(using const self, eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps) and isZeroEpsilon(width, eps) and isZeroEpsilon(height, eps)
    }

    // Returns true if this instance has a surface of zero (width and height are null)
    func isEmptyEpsilon(using const self, eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(width, eps) and isZeroEpsilon(height, eps)
    }

    // Perform a math.round operation on all the coordinates
    func round(using self)
    {
        x = acast round(x)
        y = acast round(y)
        width = acast round(width)
        height = acast round(height)
    }

    // Perform a math.trunc operation on all the coordinates
    func trunc(using self)
    {
        x = acast trunc(x)
        y = acast trunc(y)
        width = acast trunc(width)
        height = acast trunc(height)
    }

    // Perform a math.ceil operation on all the coordinates
    func ceil(using self)
    {
        x = acast ceil(x)
        y = acast ceil(y)
        width = acast ceil(width)
        height = acast ceil(height)
    }

    // Determines if the specfied point is contained within the rectangular region
    func contains(const self, x, y: f32)->bool
    {
        return (self.x >= x and x < self.x + self.width) and (self.y >= y and y < self.y + self.height)
    }

    // Determines if the specfied point is contained within the rectangular region
    func contains(using const self, pt: Point)->bool
    {
        return contains(pt.x, pt.y)
    }

    // Determines if the specfied rectangle is contained within the rectangular region
    func contains(using const self, rect: Rect)->bool
    {
        return (rect.x >= x and rect.right() <= right()) and (rect.y >= y and rect.bottom() <= bottom())
    }

    // Intersect this rectangle with another one
    func intersect(using self, with: Rect)
    {
        dref self = intersect(dref self, with)
    }

    // Creates a rectangle that represents the intersetion between 'a' and 'b'.
    // If there is no intersection, Zero is returned.
    func intersect(a, b: Rect)->Rect
    {
        x1 := max(a.x, b.x)
        x2 := min(a.x + a.width, b.x + b.width)
        y1 := max(a.y, b.y)
        y2 := min(a.y + a.height, b.y + b.height)
        if x2 >= x1 and y2 >= y1
            return Rect{x1, y1, x2 - x1, y2 - y1}
        return Zero;
    }

    // Determines if this rectangle intersets with 'rect'
    func intersectWith(using const self, rect: Rect)->bool
    {
        return (rect.x < right()) and (x < rect.right()) and (rect.y < bottom()) and (y < rect.bottom())
    }

    // Creates a rectangle that represents the union
    func setUnion(using self, with: Rect)
    {
        dref self = getUnion(dref self, with)
    }

    // Creates a rectangle that represents the union between 'a' and 'b'.
    func getUnion(a, b: Rect)->Rect
    {
        x1 := min(a.x, b.x)
        x2 := max(a.right(), b.right())
        y1 := min(a.y, b.y)
        y2 := max(a.bottom(), b.bottom())
        return Rect{x1, y1, x2 - x1, y2 - y1}
    }
}
