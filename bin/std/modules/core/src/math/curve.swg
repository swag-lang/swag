#global namespace Math.Curve
#global public

// Optimized quadratic Bézier evaluation
#[Swag.Overload]
func evaluateBezier(p0, p1, p2: Vector2, t: f32)->Vector2
{
    let it = 1.0 - t
    let it2 = it * it
    let t2 = t * t

    // Quadratic Bézier: (1-t)²P0 + 2t(1-t)P1 + t²P2
    let a = it2           // (1-t)²
    let b = 2.0 * t * it  // 2t(1-t)
    let c = t2            // t²

    return Vector2{x: a * p0.x + b * p1.x + c * p2.x, y: a * p0.y + b * p1.y + c * p2.y}
}

// Optimized Bézier evaluation
#[Swag.Overload]
func evaluateBezier(p0, p1, p2, p3: Vector2, t: f32)->Vector2
{
    let it = 1.0 - t
    let t2 = t * t
    let it2 = it * it

    // Use Horner's method for more efficient computation
    let a = it2 * it        // (1-t)³
    let b = 3.0 * t * it2   // 3t(1-t)²
    let c = 3.0 * t2 * it   // 3t²(1-t)
    let d = t2 * t          // t³

    return Vector2{x: a * p0.x + b * p1.x + c * p2.x + d * p3.x, y: a * p0.y + b * p1.y + c * p2.y + d * p3.y}
}

// Optimized cubic polynomial evaluation
// P(t) = p0*t³ + p1*t² + p2*t + p3
func evaluatePolynomial(p0, p1, p2, p3: Vector2, t: f32)->Vector2
{
    // Using Horner's method: P(t) = ((p0*t + p1)*t + p2)*t + p3
    return Vector2{x: ((p0.x * t + p1.x) * t + p2.x) * t + p3.x, y: ((p0.y * t + p1.y) * t + p2.y) * t + p3.y}
}