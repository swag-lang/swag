namespace std.math;
using swag

// A simple rectangle with 4 coordinates
struct(T) RectPriv
{
    x:      T
    y:      T
    width:  T
    height: T
}

impl RectPriv
{
    #[complete]
    func opAffect(using self, value: T)
    {
        x, y, width, height = value
    }

    func opEquals(using self, other: Self)->bool
    {
        return x == other.x && y == other.y && width == other.width && height == other.height
    }

    #[property, inline]
    {
        // right coordinate (x + width)
        func right(using self)->T
        {
            return x + width
        }

        // right coordinate (x + width)
        func right(using self, value: T)
        {
            width = value - x
        }

        // bottom coordinate (y + height)
        func bottom(using self)->T
        {
            return y + height
        }

        // bottom coordinate (y + height)
        func bottom(using self, value: T)
        {
            height = value - y
        }
    }

    // Represents a rectangle with all coordinates to zero
    //const Zero: RectPriv'T{0, 0, 0, 0}

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
    func offset(using self, value: T)
    {
        x, y += value
    }

    // Multiply the width and height of the rectangle by a given value
    func scale(using self, value: T)
    {
        width, height *= value
    }

    // Multiply the width and height of the rectangle by a given value
    func scale(using self, valueX, valueY: T)
    {
        width *= valueX
        height *= valueY
    }
}

public #bake Rect = RectPriv's32
public #bake RectF = RectPriv'f32

// Returns true if this instance is null with an epsilon
func isZeroEpsilon(using rect: RectF, eps = ConstF32.Epsilon)->bool
{
    return math.isZeroEpsilon(x, eps) && math.isZeroEpsilon(y, eps) && math.isZeroEpsilon(width, eps) && math.isZeroEpsilon(height, eps)
}

// Returns true if this instance has a surface of zero (width and heigth are null)
func isEmptyEpsilon(using rect: RectF, eps = ConstF32.Epsilon)->bool
{
    return math.isZeroEpsilon(width, eps) && math.isZeroEpsilon(height, eps)
}

// Transform a RectF to a Rect
public func roundToRect(using rect: RectF)->Rect
{
    var result: Rect = ?
    result.x = acast math.round(x)
    result.y = acast math.round(y)
    result.width = acast math.round(width)
    result.height = acast math.round(height)
    return result
}

// Transform a RectF to a Rect
public func truncToRect(using rect: RectF)->Rect
{
    var result: Rect = ?
    result.x = acast math.trunc(x)
    result.y = acast math.trunc(y)
    result.width = acast math.trunc(width)
    result.height = acast math.trunc(height)
    return result
}
