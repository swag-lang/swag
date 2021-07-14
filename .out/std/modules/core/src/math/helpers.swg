#global public
#global namespace Math

#[Swag.ConstExpr, Swag.inline]
{
    func(T) clamp(x, low, up: T) => @max(@min(x, up), low)
    func(T) min(x, y, z: T)->T   => @min(@min(x, y), z)
    func(T) max(x, y, z: T)->T   => @max(@max(x, y), z)
}

// Convert a big endian integer to current arch native format
#[Swag.ConstExpr, Swag.inline]
func(T) bigEndianToNative(x: T)->T
{
#if BIG_ENDIAN
    return x
#else
    return @byteswap(x)
}

// Convert a little endian integer to current arch native format
#[Swag.ConstExpr, Swag.inline]
func(T) littleEndianToNative(x: T)->T
{
#if BIG_ENDIAN
    return @byteswap(x)
#else
    return x
}