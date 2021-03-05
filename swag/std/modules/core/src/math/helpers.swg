#global public
#global namespace math

#[swag.constexpr, swag.inline]
{
    func(T) clamp(x, low, up: T) => x < low ? low : (x > up ? up : x)
    func(T) min(x, y: T) => x < y ? x : y
    func(T) max(x, y: T) => x > y ? x : y
    func(T) min(x, y, z: T) => min(min(x, y), z)
    func(T) max(x, y, z: T) => max(max(x, y), z)
}
