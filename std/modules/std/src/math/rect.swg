namespace std.math;

// A simple rectangle with 4 coordinates
public struct Rect
{
    x:      f32
    y:      f32
    width:  f32
    height: f32
}

public impl Rect
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

    // Returns true if this instance is null with an epsilon
    func isZeroEpsilon(using self, eps: f32 = constF32.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) && isZeroEpsilon(y, eps) && isZeroEpsilon(width, eps) && isZeroEpsilon(height, eps)
    }

    // Returns true if this instance has a surface of zero (width and heigth are null)
    func isEmpty(using self)->bool
    {
        return width == height == 0
    }

    // Returns true if this instance has a surface of zero (width and heigth are null)
    func isEmptyEpsilon(using self, eps: f32 = constF32.Epsilon)->bool
    {
        return isZeroEpsilon(width, eps) && isZeroEpsilon(height, eps)
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
}
