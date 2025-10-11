#global public
#global namespace Math

// A simple rectangle struct with position and dimensions.
// Represents an axis-aligned rectangle using top-left position and width/height dimensions.
#[Serialization.Final]
struct Rectangle
{
    x:          f32     // Left position
    y:          f32     // Top position
    width:      f32     // Rectangle width
    height:     f32     // Rectangle height
}

impl Rectangle
{
    const Zero: Rectangle{0, 0, 0, 0} // Rectangle with all coordinates set to zero

    // Check if this rectangle is exactly equal to another rectangle.
    // Performs exact floating-point comparison of all four components.
    mtd const opEquals(other: Rectangle)->bool
    {
        return .x == other.x and .y == other.y and .width == other.width and .height == other.height
    }

    // Scale the rectangle by a scalar value.
    // Multiplies all components (position and dimensions) by the given 'value'.
    mtd(op: string) const opBinary(value: f32)->Rectangle
    {
        #if op == "*" do
            return {.x * value, .y * value, .width * value, .height * value}
        #else do
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Initialize the rectangle using two corner points.
    // Automatically handles the case where points are not in top-left/bottom-right order.
    mtd set(topLeft, bottomRight: Math.Point)
    {
        .x      = Math.min(topLeft.x, bottomRight.x)
        .y      = Math.min(topLeft.y, bottomRight.y)
        .width  = Math.abs(bottomRight.x - topLeft.x)
        .height = Math.abs(bottomRight.y - topLeft.y)
    }

    // Get the horizontal center coordinate.
    // Calculated as x + width / 2.
    #[Swag.Inline]
    mtd const horzCenter()->f32
    {
        return .x + (.width * 0.5)
    }

    // Get the vertical center coordinate.
    // Calculated as y + height / 2.
    #[Swag.Inline]
    mtd const vertCenter()->f32
    {
        return .y + (.height * 0.5)
    }

    // Get the center point of the rectangle.
    // Returns a Point containing the horizontal and vertical center coordinates.
    mtd const center()->Point
    {
        return Point{.horzCenter(), .vertCenter()}
    }

    // Get the right edge coordinate.
    // Calculated as x + width.
    #[Swag.Inline]
    mtd const right()->f32
    {
        return .x + .width
    }

    // Set the right edge coordinate by adjusting the width.
    // Modifies 'width' to make the right edge equal to 'value'.
    #[Swag.Inline]
    mtd setRight(value: f32)
    {
        .width = value - .x
    }

    // Get the bottom edge coordinate.
    // Calculated as y + height.
    #[Swag.Inline]
    mtd const bottom()->f32
    {
        return .y + .height
    }

    // Set the bottom edge coordinate by adjusting the height.
    // Modifies 'height' to make the bottom edge equal to 'value'.
    #[Swag.Inline]
    mtd setBottom(value: f32)
    {
        .height = value - .y
    }

    // Move the left edge inward by the specified amount.
    // Increases 'x' and decreases 'width' by 'value', effectively shrinking from the left.
    mtd moveLeft(value: f32)
    {
        .x += value
        .width -= value
    }

    // Extend the right edge outward by the specified amount.
    // Increases 'width' by 'value', extending the rectangle to the right.
    mtd moveRight(value: f32)
    {
        .width += value
    }

    // Move the top edge downward by the specified amount.
    // Increases 'y' and decreases 'height' by 'value', effectively shrinking from the top.
    mtd moveTop(value: f32)
    {
        .y += value
        .height -= value
    }

    // Extend the bottom edge downward by the specified amount.
    // Increases 'height' by 'value', extending the rectangle downward.
    mtd moveBottom(value: f32)
    {
        .height += value
    }

    // Set all rectangle components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    mtd clear()
    {
        .x, .y, .width, .height = 0
    }

    // Returns true if all rectangle components are exactly zero.
    // Performs exact floating-point comparison, which may not work well with computed values.
    mtd const isZero()->bool
    {
        return .x == 0 and .y == 0 and .width == 0 and .height == 0
    }

    // Returns true if the rectangle has zero area.
    // A rectangle is empty if either width or height is zero.
    mtd const isEmpty()->bool
    {
        return .width == 0 or .height == 0
    }

    // Returns true if the rectangle has non-negative dimensions.
    // A normalized rectangle has width and height greater than or equal to zero.
    mtd isNormalized()->bool
    {
        return .width >= 0 and .height >= 0
    }

    // Offset the rectangle position by the same amount in both directions.
    // Adds 'offsetXY' to both x and y coordinates without changing dimensions.
    #[Swag.Overload]
    mtd offset(offsetXY: f32)
    {
        .x, .y += offsetXY
    }

    // Offset the rectangle position by separate x and y amounts.
    // Adds 'offsetX' to x coordinate and 'offsetY' to y coordinate.
    #[Swag.Overload]
    mtd offset(offsetX, offsetY: f32)
    {
        .x += offsetX
        .y += offsetY
    }

    // Offset the rectangle position by a point vector.
    // Adds the point's x and y components to the rectangle's position.
    #[Swag.Overload]
    mtd offset(offset: Math.Point)
    {
        .x += offset.x
        .y += offset.y
    }

    // Apply padding to all sides of the rectangle.
    // Uses Vector4 components as left, top, right, bottom padding values respectively.
    mtd applyPadding(padding: Math.Vector4)
    {
        .x += padding.x
        .y += padding.y
        .width -= (padding.x + padding.z)
        .height -= (padding.y + padding.w)
    }

    // Scale the rectangle dimensions uniformly.
    // Multiplies both width and height by 'value' without changing position.
    #[Swag.Overload]
    mtd scale(value: f32)
    {
        .width, .height *= value
    }

    // Scale the rectangle dimensions independently.
    // Multiplies width by 'x' and height by 'y' without changing position.
    #[Swag.Overload]
    func scale(me, x, y: f32)
    {
        .width *= x
        .height *= y
    }

    // Inflate the rectangle uniformly in all directions.
    // Expands the rectangle by 'value' on all sides, moving position and increasing dimensions.
    #[Swag.Overload]
    mtd inflate(value: f32)
    {
        .x -= value
        .y -= value
        .width += value * 2
        .height += value * 2
    }

    // Inflate the rectangle by different amounts horizontally and vertically.
    // Expands by 'x' horizontally and 'y' vertically on all sides.
    #[Swag.Overload]
    func inflate(me, x, y: f32)
    {
        .x -= x
        .y -= y
        .width += x * 2
        .height += y * 2
    }

    // Returns true if all rectangle components are zero within epsilon tolerance.
    // Uses 'eps' parameter to handle floating-point precision issues.
    mtd const isZeroEpsilon(eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(.x, eps) and isZeroEpsilon(.y, eps) and isZeroEpsilon(.width, eps) and isZeroEpsilon(.height, eps)
    }

    // Returns true if the rectangle has zero area within epsilon tolerance.
    // Checks if both width and height are zero within the specified tolerance.
    mtd const isEmptyEpsilon(eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(.width, eps) and isZeroEpsilon(.height, eps)
    }

    // Returns true if this rectangle equals 'other' within epsilon tolerance.
    // Compares all four components separately using the specified 'eps' tolerance.
    mtd const isEqualEpsilon(other: Rectangle, eps = ConstF32.Epsilon)->bool
    {
        return isEqualEpsilon(.x, other.x, eps) and
               isEqualEpsilon(.y, other.y, eps) and
               isEqualEpsilon(.width, other.width, eps) and
               isEqualEpsilon(.height, other.height, eps)
    }

    // Round all rectangle components to the nearest integer values.
    // Applies 'Math.round' function to position and dimension components in-place.
    mtd round()
    {
        .x      = Math.round(.x)
        .y      = Math.round(.y)
        .width  = Math.round(.width)
        .height = Math.round(.height)
    }

    // Truncate all rectangle components to integer values.
    // Applies 'Math.trunc' function to all components, removing fractional parts.
    mtd trunc()
    {
        .x      = Math.trunc(.x)
        .y      = Math.trunc(.y)
        .width  = Math.trunc(.width)
        .height = Math.trunc(.height)
    }

    // Round all rectangle components up to the next integer values.
    // Applies 'Math.ceil' function to all components in-place.
    mtd ceil()
    {
        .x      = Math.ceil(.x)
        .y      = Math.ceil(.y)
        .width  = Math.ceil(.width)
        .height = Math.ceil(.height)
    }

    // Test if the specified coordinates are contained within the rectangle.
    // Uses half-open interval [x, x+width) × [y, y+height) for containment test.
    #[Swag.Overload]
    func contains(const me, x, y: f32)->bool
    {
        return (x >= .x and x < .x + .width) and (y >= .y and y < .y + .height)
    }

    // Test if the specified point is contained within the rectangle.
    // Delegates to the coordinate-based 'contains' method.
    #[Swag.Overload]
    mtd const contains(pt: Point)->bool
    {
        return .contains(pt.x, pt.y)
    }

    // Test if the specified rectangle is entirely contained within this rectangle.
    // Returns true only if 'rect' is completely inside the bounds of this rectangle.
    #[Swag.Overload]
    mtd const contains(rect: Rectangle)->bool
    {
        return (rect.x >= .x and rect.right() <= .right()) and (rect.y >= .y and rect.bottom() <= .bottom())
    }

    // Intersect this rectangle with another rectangle in-place.
    // Modifies this rectangle to contain only the overlapping area with 'other'.
    #[Swag.Overload]
    mtd intersect(other: Rectangle)
    {
        dref me = intersect(dref me, other)
    }

    // Create a rectangle representing the intersection of two rectangles.
    // Returns the overlapping area between 'a' and 'b', or 'Zero' if no intersection exists.
    #[Swag.Overload]
    func intersect(a, b: Rectangle)->Rectangle
    {
        let x1 = max(a.x, b.x)
        let x2 = min(a.x + a.width, b.x + b.width)
        let y1 = max(a.y, b.y)
        let y2 = min(a.y + a.height, b.y + b.height)
        if x2 >= x1 and y2 >= y1 do
            return Rectangle{x1, y1, x2 - x1, y2 - y1}
        return Zero
    }

    // Test if this rectangle intersects with another rectangle.
    // Returns true if there is any overlap between the two rectangles.
    mtd const intersectWith(rect: Rectangle)->bool
    {
        return (rect.x < .right()) and (.x < rect.right()) and (rect.y < .bottom()) and (.y < rect.bottom())
    }

    // Set this rectangle to the union with another rectangle.
    // Modifies this rectangle to be the smallest rectangle containing both this and 'other'.
    mtd setUnion(other: Rectangle)
    {
        dref me = getUnion(dref me, other)
    }

    // Create a rectangle representing the union of two rectangles.
    // Returns the smallest rectangle that completely contains both 'a' and 'b'.
    func getUnion(a, b: Rectangle)->Rectangle
    {
        let x1 = min(a.x, b.x)
        let x2 = max(a.right(), b.right())
        let y1 = min(a.y, b.y)
        let y2 = max(a.bottom(), b.bottom())
        return Rectangle{x1, y1, x2 - x1, y2 - y1}
    }

    // Get the area of the rectangle.
    // Calculated as width × height, may be negative for non-normalized rectangles.
    #[Swag.Inline]
    mtd const area()->f32
    {
        return .width * .height
    }

    // Get the perimeter of the rectangle.
    // Calculated as 2 × (width + height).
    #[Swag.Inline]
    mtd const perimeter()->f32
    {
        return 2.0 * (.width + .height)
    }

    // Get the aspect ratio of the rectangle.
    // Returns width / height, or 0 if height is zero.
    mtd const aspectRatio()->f32
    {
        if .height == 0 do
            return 0
        return .width / .height
    }

    // Normalize the rectangle to ensure positive width and height.
    // Adjusts position and dimensions if width or height are negative.
    mtd normalize()
    {
        if .width < 0
        {
            .x += .width
            .width = -.width
        }
        if .height < 0
        {
            .y += .height
            .height = -.height
        }
    }

    // Get a normalized copy of the rectangle.
    // Returns a new rectangle with positive width and height without modifying the original.
    mtd const toNormalized()->Rectangle
    {
        var result = dref me
        result.normalize()
        return result
    }

    // Get the top-left corner point.
    // Returns a Point containing the x and y coordinates.
    #[Swag.Inline]
    mtd const topLeft()->Point
    {
        return Point{.x, .y}
    }

    // Get the top-right corner point.
    // Returns a Point at (x + width, y).
    #[Swag.Inline]
    mtd const topRight()->Point
    {
        return Point{.x + .width, .y}
    }

    // Get the bottom-left corner point.
    // Returns a Point at (x, y + height).
    #[Swag.Inline]
    mtd const bottomLeft()->Point
    {
        return Point{.x, .y + .height}
    }

    // Get the bottom-right corner point.
    // Returns a Point at (x + width, y + height).
    #[Swag.Inline]
    mtd const bottomRight()->Point
    {
        return Point{.x + .width, .y + .height}
    }

    // Get the size of the rectangle as a point.
    // Returns a Point containing width and height values.
    #[Swag.Inline]
    mtd const size()->Point
    {
        return Point{.width, .height}
    }

    // Set the size of the rectangle.
    // Modifies width and height without changing position.
    mtd setSize(newSize: Point)
    {
        .width  = newSize.x
        .height = newSize.y
    }

    // Clamp a point to be within the rectangle bounds.
    // Returns the closest point inside or on the rectangle boundary.
    mtd const clampPoint(pt: Point)->Point
    {
        return Point{Math.clamp(pt.x, .x, .x + .width), Math.clamp(pt.y, .y, .y + .height)}
    }

    // Get the distance from a point to the rectangle.
    // Returns 0 if the point is inside the rectangle, otherwise returns the shortest distance to the boundary.
    mtd const distanceToPoint(pt: Point)->f32
    {
        let clamped = .clampPoint(pt)
        let dx      = pt.x - clamped.x
        let dy      = pt.y - clamped.y
        return Math.sqrt(dx * dx + dy * dy)
    }

    // Fit this rectangle within bounds while maintaining aspect ratio.
    // Scales the rectangle to fit inside 'bounds' without distortion.
    mtd fitWithin(bounds: Rectangle)
    {
        if .width == 0 or .height == 0 do
            return

        let scaleX = bounds.width / .width
        let scaleY = bounds.height / .height
        let scale  = Math.min(scaleX, scaleY)

        .width *= scale
        .height *= scale
        .x = bounds.x + (bounds.width - .width) * 0.5
        .y = bounds.y + (bounds.height - .height) * 0.5
    }

    // Create a rectangle that fits within bounds while maintaining aspect ratio.
    // Returns a new rectangle scaled to fit inside 'bounds' without distortion.
    func createFittedWithin(rect: Rectangle, bounds: Rectangle)->Rectangle
    {
        var result = rect
        result.fitWithin(bounds)
        return result
    }

    // Expand the rectangle to include a point.
    // Grows the rectangle if necessary to contain the specified point.
    #[Swag.Overload]
    mtd expandToInclude(pt: Point)
    {
        let right     = .x + .width
        let bottom    = .y + .height
        let newLeft   = Math.min(.x, pt.x)
        let newTop    = Math.min(.y, pt.y)
        let newRight  = Math.max(right, pt.x)
        let newBottom = Math.max(bottom, pt.y)

        .x      = newLeft
        .y      = newTop
        .width  = newRight - newLeft
        .height = newBottom - newTop
    }

    // Expand the rectangle to include another rectangle.
    // Grows the rectangle if necessary to contain the entire specified rectangle.
    #[Swag.Overload]
    mtd expandToInclude(other: Rectangle)
    {
        .expandToInclude(other.topLeft())
        .expandToInclude(other.bottomRight())
    }
}
