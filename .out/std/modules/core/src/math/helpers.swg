#global public
#global namespace math

#[swag.constexpr, swag.inline]
{
    func(T) clamp(x, low, up: T)->T => @max(@min(x, up), low)
    func(T) min(x, y, z: T)->T      => @min(@min(x, y), z)
    func(T) max(x, y, z: T)->T      => @max(@max(x, y), z)
}
