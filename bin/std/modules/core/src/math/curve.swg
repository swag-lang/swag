#global namespace Math
#global namespace Curve
#global public

// Evaluate a cubic polynomial at parameter 't'.
// Uses Horner's method for efficient computation: P(t) = ((p0*t + p1)*t + p2)*t + p3.
func evaluatePolynomial(p0, p1, p2, p3: Vector2, t: f32)->Vector2
{
    // Using Horner's method: P(t) = ((p0*t + p1)*t + p2)*t + p3
    return Vector2{x: ((p0.x * t + p1.x) * t + p2.x) * t + p3.x, y: ((p0.y * t + p1.y) * t + p2.y) * t + p3.y}
}

// Evaluate a quadratic Bézier curve at parameter 't'.
// Formula: (1-t)²start + 2t(1-t)ctrl + t²end where 't' ranges from 0.0 to 1.0.
#[Swag.Overload]
func evaluateBezier(start, end, ctrl: Vector2, t: f32)->Vector2
{
    let it  = 1.0 - t
    let it2 = it * it
    let t2  = t * t

    // Quadratic Bézier: (1-t)²start + 2t(1-t)end + t²P2
    let a = it2              // (1-t)²
    let b = 2.0 * t * it     // 2t(1-t)
    let c = t2               // t²

    return Vector2{x: a * start.x + b * end.x + c * ctrl.x, y: a * start.y + b * end.y + c * ctrl.y}
}

// Evaluate a cubic Bézier curve at parameter 't'.
// Formula: (1-t)³start + 3t(1-t)²ctrl1 + 3t²(1-t)ctrl2 + t³end where 't' ranges from 0.0 to 1.0.
#[Swag.Overload]
func evaluateBezier(start, end, ctrl1, ctrl2: Vector2, t: f32)->Vector2
{
    let it  = 1.0 - t
    let t2  = t * t
    let it2 = it * it

    // Use Horner's method for more efficient computation
    let a = it2 * it          // (1-t)³
    let b = 3.0 * t * it2     // 3t(1-t)²
    let c = 3.0 * t2 * it     // 3t²(1-t)
    let d = t2 * t            // t³

    return Vector2{x: a * start.x + b * end.x + c * ctrl1.x + d * ctrl2.x, y: a * start.y + b * end.y + c * ctrl1.y + d * ctrl2.y}
}

// Find roots of quadratic equation ax² + bx + c = 0 within the range [0, 1].
// Returns an array containing valid roots that fall within the parameter range.
private func solveQuadratic(a: f32, b: f32, c: f32)->Array'f32
{
    var roots: Array'f32

    if Math.abs(a) < 1e-6
    {
        // Linear case: bx + c = 0
        if Math.abs(b) > 1e-6
        {
            let root = -c / b
            if root >= 0.0 and root <= 1.0 do
                roots.add(root)
        }
    }
    else
    {
        // Quadratic case
        let discriminant = b * b - 4.0 * a * c
        if discriminant >= 0.0
        {
            let sqrtD = Math.sqrt(discriminant)
            let root1 = (-b + sqrtD) / (2.0 * a)
            let root2 = (-b - sqrtD) / (2.0 * a)

            if root1 >= 0.0 and root1 <= 1.0 do
                roots.add(root1)
            if root2 >= 0.0 and root2 <= 1.0 do
                roots.add(root2)
        }
    }

    return roots
}

// Calculate the axis-aligned bounding box of a cubic Bézier curve.
// Finds extrema by solving the derivative equation and evaluating the curve at critical points.
#[Swag.Overload]
func getBezierBoundingBox(start, end, ctrl0, ctrl1: Vector2)->Rectangle
{
    // Start with endpoints
    var minX = Math.min(start.x, end.x)
    var maxX = Math.max(start.x, end.x)
    var minY = Math.min(start.y, end.y)
    var maxY = Math.max(start.y, end.y)

    // Find extrema by solving derivative = 0
    // For cubic Bézier B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
    // Derivative B'(t) = 3[(1-t)²(P₁-P₀) + 2(1-t)t(P₂-P₁) + t²(P₃-P₂)]
    // Which simplifies to: B'(t) = at² + bt + c where:

    // For X component:
    let ax = 3.0 * (end.x - 3.0 * ctrl1.x + 3.0 * ctrl0.x - start.x)
    let bx = 6.0 * (ctrl1.x - 2.0 * ctrl0.x + start.x)
    let cx = 3.0 * (ctrl0.x - start.x)

    let xRoots = solveQuadratic(ax, bx, cx)
    foreach t in xRoots
    {
        let point = evaluateBezier(start, ctrl0, ctrl1, end, t)
        minX = Math.min(minX, point.x)
        maxX = Math.max(maxX, point.x)
    }

    // For Y component:
    let ay = 3.0 * (end.y - 3.0 * ctrl1.y + 3.0 * ctrl0.y - start.y)
    let by = 6.0 * (ctrl1.y - 2.0 * ctrl0.y + start.y)
    let cy = 3.0 * (ctrl0.y - start.y)

    let yRoots = solveQuadratic(ay, by, cy)
    foreach t in yRoots
    {
        let point = evaluateBezier(start, ctrl0, ctrl1, end, t)
        minY = Math.min(minY, point.y)
        maxY = Math.max(maxY, point.y)
    }

    return Rectangle{minX, minY, maxX - minX, maxY - minY}
}

// Calculate the axis-aligned bounding box of a quadratic Bézier curve.
// Finds extrema by solving the linear derivative equation for critical points.
#[Swag.Overload]
func getBezierBoundingBox(start, end, ctrl: Vector2)->Rectangle
{
    // Start with endpoints
    var minX = Math.min(start.x, end.x)
    var maxX = Math.max(start.x, end.x)
    var minY = Math.min(start.y, end.y)
    var maxY = Math.max(start.y, end.y)

    // Find extrema by solving derivative = 0
    // For quadratic Bézier B(t) = (1-t)²P₀ + 2(1-t)tP₁ + t²P₂
    // Derivative B'(t) = 2[(1-t)(P₁-P₀) + t(P₂-P₁)]
    // Which simplifies to: B'(t) = 2[P₀ - 2P₁ + P₂]t + 2[P₁ - P₀]
    // Setting to zero: at + b = 0, where a = 2(P₀ - 2P₁ + P₂), b = 2(P₁ - P₀)

    // For X component:
    let ax = 2.0 * (start.x - 2.0 * ctrl.x + end.x)
    let bx = 2.0 * (ctrl.x - start.x)

    if Math.abs(ax) > 1e-6
    {
        let t = -bx / ax
        if t >= 0.0 and t <= 1.0
        {
            let point = evaluateBezier(start, ctrl, end, t)
            minX = Math.min(minX, point.x)
            maxX = Math.max(maxX, point.x)
        }
    }

    // For Y component:
    let ay = 2.0 * (start.y - 2.0 * ctrl.y + end.y)
    let by = 2.0 * (ctrl.y - start.y)

    if Math.abs(ay) > 1e-6
    {
        let t = -by / ay
        if t >= 0.0 and t <= 1.0
        {
            let point = evaluateBezier(start, ctrl, end, t)
            minY = Math.min(minY, point.y)
            maxY = Math.max(maxY, point.y)
        }
    }

    return Rectangle{minX, minY, maxX - minX, maxY - minY}
}
