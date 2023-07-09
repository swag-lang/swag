#global public
#global namespace Math

// A simple rectangle with 4 coordinates
#[Serialization.PodFinal]
struct Rectangle
{
    x:      f32     // Left position
    y:      f32     // Top position
    width:  f32     // Rectangle width
    height: f32     // Rectangle height
}

impl Rectangle
{
    // Represents a rectangle with all coordinates to zero
    const Zero: Rectangle{0, 0, 0, 0}

    mtd const opEquals(other: Rectangle)->bool
    {
        return x == other.x and y == other.y and width == other.width and height == other.height
    }

    // Initialize the rectangle with two position
    mtd set(topLeft, bottomRight: Math.Point)
    {
        x = Math.min(topLeft.x, bottomRight.x)
        y = Math.min(topLeft.y, bottomRight.y)
        width = Math.abs(bottomRight.x - topLeft.x)
        height = Math.abs(bottomRight.y - topLeft.y)
    }

    // Horizontal center coordinate (x + width / 2)
    #[Swag.Inline]
    mtd const horzCenter()->f32
    {
        return x + (width * 0.5)
    }

    // Vertical center coordinate (y + height / 2)
    #[Swag.Inline]
    mtd const vertCenter()->f32
    {
        return y + (height * 0.5)
    }

    // Central point
    mtd const center()->Point
    {
        return Point{horzCenter(), vertCenter()}
    }

    // Right coordinate (x + width)
    #[Swag.Inline]
    mtd const right()->f32
    {
        return x + width
    }

    // Right coordinate (x + width)
    #[Swag.Inline]
    mtd setRight(value: f32)
    {
        width = value - x
    }

    // Bottom coordinate (y + height)
    #[Swag.Inline]
    mtd const bottom()->f32
    {
        return y + height
    }

    // Bottom coordinate (y + height)
    #[Swag.Inline]
    mtd setBottom(value: f32)
    {
        height = value - y
    }

    // Move the left coordinate, reducing the width
    mtd moveLeft(value: f32)
    {
        x += value
        width -= value
    }

    // Move the left coordinate, reducing the width
    mtd moveRight(value: f32)
    {
        width += value
    }

    // Move the left coordinate, reducing the width
    mtd moveTop(value: f32)
    {
        y += value
        height -= value
    }

    // Move the left coordinate, reducing the width
    mtd moveBottom(value: f32)
    {
        height += value
    }

    // Set rectangle to (0, 0, 0, 0)
    mtd clear()
    {
        x, y, width, height = 0
    }

    // Returns true if this instance is null
    mtd const isZero()->bool
    {
        return x == 0 and y == 0 and width == 0 and height == 0
    }

    // Returns true if this instance has a surface of zero (width or height are null)
    mtd const isEmpty()->bool
    {
        return width == 0 or height == 0
    }

    // Returns true if this instance has a positive or null surface (width and height greater or equal than zero)
    mtd isNormalized()->bool
    {
        return width >= 0 and height >= 0
    }

    // Offset the rectangle position by a given value
    mtd offset(offsetXY: f32)
    {
        x, y += offsetXY
    }

    // Offset the rectangle position by a x and y values
    mtd offset(offsetX, offsetY: f32)
    {
        x += offsetX
        y += offsetY
    }

    // Offset the rectangle position by a given value
    mtd offset(offset: Math.Point)
    {
        x += offset.x
        y += offset.y
    }

    // Add a padding (offset to each side)
    mtd applyPadding(padding: Math.Vector4)
    {
        x += padding.x
        y += padding.y
        width -= (padding.x + padding.z)
        height -= (padding.y + padding.w)
    }

    // Multiply the width and height of the rectangle by a given 'value'
    mtd scale(value: f32)
    {
        width, height *= value
    }

    // Multiply the width by 'x' and the height by 'y'
    func scale(self, x, y: f32)
    {
        self.width *= x
        self.height *= y
    }

    // Inflates the rectangle by the given 'value'
    mtd inflate(value: f32)
    {
        x -= value
        y -= value
        width += value * 2
        height += value * 2
    }

    // Inflates the rectangle by the given amount in 'x' and 'y' directions
    func inflate(self, x, y: f32)
    {
        self.x -= x
        self.y -= y
        self.width += x * 2
        self.height += y * 2
    }

    // Returns true if this instance is null with an epsilon
    mtd const isZeroEpsilon(eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps) and isZeroEpsilon(width, eps) and isZeroEpsilon(height, eps)
    }

    // Returns true if this instance has a surface of zero (width and height are null)
    mtd const isEmptyEpsilon(eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(width, eps) and isZeroEpsilon(height, eps)
    }

    // Perform a Math.round operation on all the coordinates
    mtd round()
    {
        x = acast round(x)
        y = acast round(y)
        width = acast round(width)
        height = acast round(height)
    }

    // Perform a Math.trunc operation on all the coordinates
    mtd trunc()
    {
        x = acast trunc(x)
        y = acast trunc(y)
        width = acast trunc(width)
        height = acast trunc(height)
    }

    // Perform a Math.ceil operation on all the coordinates
    mtd ceil()
    {
        x = acast ceil(x)
        y = acast ceil(y)
        width = acast ceil(width)
        height = acast ceil(height)
    }

    // Determines if the specfied point is contained within the rectangular region
    func contains(const self, x, y: f32)->bool
    {
        return (x >= self.x and x < self.x + self.width) and (y >= self.y and y < self.y + self.height)
    }

    // Determines if the specfied point is contained within the rectangular region
    mtd const contains(pt: Point)->bool
    {
        return contains(pt.x, pt.y)
    }

    // Determines if the specfied rectangle is contained within the rectangular region
    mtd const contains(rect: Rectangle)->bool
    {
        return (rect.x >= x and rect.right() <= right()) and (rect.y >= y and rect.bottom() <= bottom())
    }

    // Intersect this rectangle with another one
    mtd intersect(other: Rectangle)
    {
        dref self = intersect(dref self, other)
    }

    // Creates a rectangle that represents the intersetion between 'a' and 'b'.
    // If there is no intersection, Zero is returned.
    func intersect(a, b: Rectangle)->Rectangle
    {
        let x1 = max(a.x, b.x)
        let x2 = min(a.x + a.width, b.x + b.width)
        let y1 = max(a.y, b.y)
        let y2 = min(a.y + a.height, b.y + b.height)
        if x2 >= x1 and y2 >= y1
            return Rectangle{x1, y1, x2 - x1, y2 - y1}
        return Zero;
    }

    // Determines if this rectangle intersets with 'rect'
    mtd const intersectWith(rect: Rectangle)->bool
    {
        return (rect.x < right()) and (x < rect.right()) and (rect.y < bottom()) and (y < rect.bottom())
    }

    // Creates a rectangle that represents the union
    mtd setUnion(other: Rectangle)
    {
        dref self = getUnion(dref self, other)
    }

    // Creates a rectangle that represents the union between 'a' and 'b'.
    func getUnion(a, b: Rectangle)->Rectangle
    {
        let x1 = min(a.x, b.x)
        let x2 = max(a.right(), b.right())
        let y1 = min(a.y, b.y)
        let y2 = max(a.bottom(), b.bottom())
        return Rectangle{x1, y1, x2 - x1, y2 - y1}
    }

    // Constrain the rectangle to be inside another one, without changing its size
    // (so the other rectangle must be bigger)
    mtd constrainIn(other: Rectangle)
    {
        if x < other.x
            x += other.x - x
        if y < other.y
            y += other.y - y
        if right() > other.right()
            x -= right() - other.right()
        if bottom() > other.bottom()
            y -= bottom() - other.bottom()
    }
}
