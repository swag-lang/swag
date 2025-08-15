#global marked
#global public
#global namespace Math

// A simple rectangle struct with position and dimensions.
// Represents an axis-aligned rectangle using top-left position and self.width/height dimensions.
#[Serialization.Final]
struct Rectangle
{
    x:          f32     // Left position
    y:          f32     // Top position
    width:      f32     // Rectangle self.width
    height:     f32     // Rectangle self.height
}

impl Rectangle
{
    const Zero: Rectangle{0, 0, 0, 0} // Rectangle with all coordinates set to zero

    // Check if this rectangle is exactly equal to another rectangle.
    // Performs exact floating-point comparison of all four components.
    mtd const opEquals(other: Rectangle)->bool
    {
        return self.x == other.x and self.y == other.y and self.width == other.width and self.height == other.height
    }

    // Scale the rectangle by a scalar value.
    // Multiplies all components (position and dimensions) by the given 'value'.
    mtd(op: string) const opBinary(value: f32)->Rectangle
    {
        #if op == "*":
            return {x * value, self.y * value, self.width * value, self.height * value}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Initialize the rectangle using two corner points.
    // Automatically handles the case where points are not in top-left/bottom-right order.
    mtd set(topLeft, bottomRight: Math.Point)
    {
        self.x      = Math.min(topLeft.x, bottomRight.x)
        self.y      = Math.min(topLeft.y, bottomRight.y)
        self.width  = Math.abs(bottomRight.x - topLeft.x)
        self.height = Math.abs(bottomRight.y - topLeft.y)
    }

    // Get the horizontal center coordinate.
    // Calculated as self.x + self.width / 2.
    #[Swag.Inline]
    mtd const horzCenter()->f32
    {
        return self.x + (self.width * 0.5)
    }

    // Get the vertical center coordinate.
    // Calculated as self.y + self.height / 2.
    #[Swag.Inline]
    mtd const vertCenter()->f32
    {
        return self.y + (self.height * 0.5)
    }

    // Get the center point of the rectangle.
    // Returns a Point containing the horizontal and vertical center coordinates.
    mtd const center()->Point
    {
        return Point{self.horzCenter(), self.vertCenter()}
    }

    // Get the right edge coordinate.
    // Calculated as self.x + self.width.
    #[Swag.Inline]
    mtd const right()->f32
    {
        return self.x + self.width
    }

    // Set the right edge coordinate by adjusting the self.width.
    // Modifies 'width' to make the right edge equal to 'value'.
    #[Swag.Inline]
    mtd setRight(value: f32)
    {
        self.width = value - self.x
    }

    // Get the bottom edge coordinate.
    // Calculated as self.y + self.height.
    #[Swag.Inline]
    mtd const bottom()->f32
    {
        return self.y + self.height
    }

    // Set the bottom edge coordinate by adjusting the self.height.
    // Modifies 'height' to make the bottom edge equal to 'value'.
    #[Swag.Inline]
    mtd setBottom(value: f32)
    {
        self.height = value - self.y
    }

    // Move the left edge inward by the specified amount.
    // Increases 'x' and decreases 'width' by 'value', effectively shrinking from the left.
    mtd moveLeft(value: f32)
    {
        self.x += value
        self.width -= value
    }

    // Extend the right edge outward by the specified amount.
    // Increases 'width' by 'value', extending the rectangle to the right.
    mtd moveRight(value: f32)
    {
        self.width += value
    }

    // Move the top edge downward by the specified amount.
    // Increases 'y' and decreases 'height' by 'value', effectively shrinking from the top.
    mtd moveTop(value: f32)
    {
        self.y += value
        self.height -= value
    }

    // Extend the bottom edge downward by the specified amount.
    // Increases 'height' by 'value', extending the rectangle downward.
    mtd moveBottom(value: f32)
    {
        self.height += value
    }

    // Set all rectangle components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    mtd clear()
    {
        self.x, self.y, self.width, self.height = 0
    }

    // Returns true if all rectangle components are exactly zero.
    // Performs exact floating-point comparison, which may not work well with computed values.
    mtd const isZero()->bool
    {
        return self.x == 0 and self.y == 0 and self.width == 0 and self.height == 0
    }

    // Returns true if the rectangle has zero area.
    // A rectangle is empty if either self.width or self.height is zero.
    mtd const isEmpty()->bool
    {
        return self.width == 0 or self.height == 0
    }

    // Returns true if the rectangle has non-negative dimensions.
    // A normalized rectangle has self.width and self.height greater than or equal to zero.
    mtd isNormalized()->bool
    {
        return self.width >= 0 and self.height >= 0
    }

    // Offset the rectangle position by the same amount in both directions.
    // Adds 'offsetXY' to both self.x and self.y coordinates without changing dimensions.
    #[Swag.Overload]
    mtd offset(offsetXY: f32)
    {
        self.x, self.y += offsetXY
    }

    // Offset the rectangle position by separate self.x and self.y amounts.
    // Adds 'offsetX' to self.x coordinate and 'offsetY' to self.y coordinate.
    #[Swag.Overload]
    mtd offset(offsetX, offsetY: f32)
    {
        self.x += offsetX
        self.y += offsetY
    }

    // Offset the rectangle position by a point vector.
    // Adds the point's self.x and self.y components to the rectangle's position.
    #[Swag.Overload]
    mtd offset(offset: Math.Point)
    {
        self.x += offset.x
        self.y += offset.y
    }

    // Apply padding to all sides of the rectangle.
    // Uses Vector4 components as left, top, right, bottom padding values respectively.
    mtd applyPadding(padding: Math.Vector4)
    {
        self.x += padding.x
        self.y += padding.y
        self.width -= (padding.x + padding.z)
        self.height -= (padding.y + padding.w)
    }

    // Scale the rectangle dimensions uniformly.
    // Multiplies both self.width and self.height by 'value' without changing position.
    #[Swag.Overload]
    mtd scale(value: f32)
    {
        self.width, self.height *= value
    }

    // Scale the rectangle dimensions independently.
    // Multiplies self.width by 'x' and self.height by 'y' without changing position.
    #[Swag.Overload]
    mtd scale(x, y: f32)
    {
        self.width *= x
        self.height *= y
    }

    // Inflate the rectangle uniformly in all directions.
    // Expands the rectangle by 'value' on all sides, moving position and increasing dimensions.
    #[Swag.Overload]
    mtd inflate(value: f32)
    {
        self.x -= value
        self.y -= value
        self.width += value * 2
        self.height += value * 2
    }

    // Inflate the rectangle by different amounts horizontally and vertically.
    // Expands by 'x' horizontally and 'y' vertically on all sides.
    #[Swag.Overload]
    mtd inflate(x, y: f32)
    {
        self.x -= x
        self.y -= y
        self.width += x * 2
        self.height += y * 2
    }

    // Returns true if all rectangle components are zero within epsilon tolerance.
    // Uses 'eps' parameter to handle floating-point precision issues.
    mtd const isZeroEpsilon(eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(self.x, eps) and isZeroEpsilon(self.y, eps) and isZeroEpsilon(self.width, eps) and isZeroEpsilon(self.height, eps)
    }

    // Returns true if the rectangle has zero area within epsilon tolerance.
    // Checks if both self.width and self.height are zero within the specified tolerance.
    mtd const isEmptyEpsilon(eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(self.width, eps) and isZeroEpsilon(self.height, eps)
    }

    // Returns true if this rectangle equals 'other' within epsilon tolerance.
    // Compares all four components separately using the specified 'eps' tolerance.
    mtd const isEqualEpsilon(other: Rectangle, eps = ConstF32.Epsilon)->bool
    {
        return isEqualEpsilon(self.x, other.x, eps) and
               isEqualEpsilon(self.y, other.y, eps) and
               isEqualEpsilon(self.width, other.width, eps) and
               isEqualEpsilon(self.height, other.height, eps)
    }

    // Round all rectangle components to the nearest integer values.
    // Applies 'Math.round' function to position and dimension components in-place.
    mtd round()
    {
        self.x      = Math.round(self.x)
        self.y      = Math.round(self.y)
        self.width  = Math.round(self.width)
        self.height = Math.round(self.height)
    }

    // Truncate all rectangle components to integer values.
    // Applies 'Math.trunc' function to all components, removing fractional parts.
    mtd trunc()
    {
        self.x      = Math.trunc(self.x)
        self.y      = Math.trunc(self.y)
        self.width  = Math.trunc(self.width)
        self.height = Math.trunc(self.height)
    }

    // Round all rectangle components up to the next integer values.
    // Applies 'Math.ceil' function to all components in-place.
    mtd ceil()
    {
        self.x      = Math.ceil(self.x)
        self.y      = Math.ceil(self.y)
        self.width  = Math.ceil(self.width)
        self.height = Math.ceil(self.height)
    }

    // Test if the specified coordinates are contained within the rectangle.
    // Uses half-open interval [x, x+width) × [y, y+height) for containment test.
    #[Swag.Overload]
    mtd const contains(x, y: f32)->bool
    {
        return (x >= self.x and x < self.x + self.width) and (y >= self.y and y < self.y + self.height)
    }

    // Test if the specified point is contained within the rectangle.
    // Delegates to the coordinate-based 'contains' method.
    #[Swag.Overload]
    mtd const contains(pt: Point)->bool
    {
        return self.contains(pt.x, pt.y)
    }

    // Test if the specified rectangle is entirely contained within this rectangle.
    // Returns true only if 'rect' is completely inside the bounds of this rectangle.
    #[Swag.Overload]
    mtd const contains(rect: Rectangle)->bool
    {
        return (rect.x >= self.x and rect.right() <= self.right()) and (rect.y >= self.y and rect.bottom() <= self.bottom())
    }

    // Intersect this rectangle with another rectangle in-place.
    // Modifies this rectangle to contain only the overlapping area with 'other'.
    #[Swag.Overload]
    mtd intersect(other: Rectangle)
    {
        dref self = intersect(dref self, other)
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
        if x2 >= x1 and y2 >= y1:
            return Rectangle{x1, y1, x2 - x1, y2 - y1}
        return Zero
    }

    // Test if this rectangle intersects with another rectangle.
    // Returns true if there is any overlap between the two rectangles.
    mtd const intersectWith(rect: Rectangle)->bool
    {
        return (rect.x < self.right()) and (self.x < rect.right()) and (rect.y < self.bottom()) and (self.y < rect.bottom())
    }

    // Set this rectangle to the union with another rectangle.
    // Modifies this rectangle to be the smallest rectangle containing both this and 'other'.
    mtd setUnion(other: Rectangle)
    {
        dref self = getUnion(dref self, other)
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
    // Calculated as self.width × self.height, may be negative for non-normalized rectangles.
    #[Swag.Inline]
    mtd const area()->f32
    {
        return self.width * self.height
    }

    // Get the perimeter of the rectangle.
    // Calculated as 2 × (width + self.height).
    #[Swag.Inline]
    mtd const perimeter()->f32
    {
        return 2.0 * (self.width + self.height)
    }

    // Get the aspect ratio of the rectangle.
    // Returns self.width / self.height, or 0 if self.height is zero.
    mtd const aspectRatio()->f32
    {
        if self.height == 0:
            return 0
        return self.width / self.height
    }

    // Normalize the rectangle to ensure positive self.width and self.height.
    // Adjusts position and dimensions if self.width or self.height are negative.
    mtd normalize()
    {
        if self.width < 0
        {
            self.x += self.width
            self.width = -self.width
        }
        if self.height < 0
        {
            self.y += self.height
            self.height = -self.height
        }
    }

    // Get a normalized copy of the rectangle.
    // Returns a new rectangle with positive self.width and self.height without modifying the original.
    mtd const toNormalized()->Rectangle
    {
        var result = dref self
        result.normalize()
        return result
    }

    // Get the top-left corner point.
    // Returns a Point containing the self.x and self.y coordinates.
    #[Swag.Inline]
    mtd const topLeft()->Point
    {
        return Point{self.x, self.y}
    }

    // Get the top-right corner point.
    // Returns a Point at (x + self.width, self.y).
    #[Swag.Inline]
    mtd const topRight()->Point
    {
        return Point{self.x + self.width, self.y}
    }

    // Get the bottom-left corner point.
    // Returns a Point at (x, self.y + self.height).
    #[Swag.Inline]
    mtd const bottomLeft()->Point
    {
        return Point{self.x, self.y + self.height}
    }

    // Get the bottom-right corner point.
    // Returns a Point at (x + self.width, self.y + self.height).
    #[Swag.Inline]
    mtd const bottomRight()->Point
    {
        return Point{self.x + self.width, self.y + self.height}
    }

    // Get the size of the rectangle as a point.
    // Returns a Point containing self.width and self.height values.
    #[Swag.Inline]
    mtd const size()->Point
    {
        return Point{self.width, self.height}
    }

    // Set the size of the rectangle.
    // Modifies self.width and self.height without changing position.
    mtd setSize(newSize: Point)
    {
        self.width  = newSize.x
        self.height = newSize.y
    }

    // Clamp a point to be within the rectangle bounds.
    // Returns the closest point inside or on the rectangle boundary.
    mtd const clampPoint(pt: Point)->Point
    {
        return Point{Math.clamp(pt.x, self.x, self.x + self.width), Math.clamp(pt.y, self.y, self.y + self.height)}
    }

    // Get the distance from a point to the rectangle.
    // Returns 0 if the point is inside the rectangle, otherwise returns the shortest distance to the boundary.
    mtd const distanceToPoint(pt: Point)->f32
    {
        let clamped = self.clampPoint(pt)
        let dx      = pt.x - clamped.x
        let dy      = pt.y - clamped.y
        return Math.sqrt(dx * dx + dy * dy)
    }

    // Fit this rectangle within bounds while maintaining aspect ratio.
    // Scales the rectangle to fit inside 'bounds' without distortion.
    mtd fitWithin(bounds: Rectangle)
    {
        if self.width == 0 or self.height == 0:
            return

        let scaleX = bounds.width / self.width
        let scaleY = bounds.height / self.height
        let scale  = Math.min(scaleX, scaleY)

        self.width *= scale
        self.height *= scale
        self.x = bounds.x + (bounds.width - self.width) * 0.5
        self.y = bounds.y + (bounds.height - self.height) * 0.5
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
        let right     = self.x + self.width
        let bottom    = self.y + self.height
        let newLeft   = Math.min(self.x, pt.x)
        let newTop    = Math.min(self.y, pt.y)
        let newRight  = Math.max(right, pt.x)
        let newBottom = Math.max(bottom, pt.y)

        self.x      = newLeft
        self.y      = newTop
        self.width  = newRight - newLeft
        self.height = newBottom - newTop
    }

    // Expand the rectangle to include another rectangle.
    // Grows the rectangle if necessary to contain the entire specified rectangle.
    #[Swag.Overload]
    mtd expandToInclude(other: Rectangle)
    {
        self.expandToInclude(other.topLeft())
        self.expandToInclude(other.bottomRight())
    }
}
