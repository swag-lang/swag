public namespace math;

#[swag.constexpr, swag.inline]
{
    func(T) clamp(x, low, up: T) => x < low ? low : (x > up ? up : x)
    func(T) min(x, y: T) => x < y ? x : y
    func(T) max(x, y: T) => x > y ? x : y
}
