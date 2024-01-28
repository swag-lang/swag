#global public
#global namespace Math

#[Swag.ConstExpr, Swag.Inline]
{
    func(T) clamp(x, low, up: T) => @max(@min(x, up), low)
    func(T) min(x, y, z: T)->T   => @min(@min(x, y), z)
    func(T) max(x, y, z: T)->T   => @max(@max(x, y), z)
}

// Convert a big endian integer to current arch native format
#[Swag.ConstExpr, Swag.Inline]
func(T) bigEndianToNative(x: T)->T
{
#if BIG_ENDIAN #do
    return x
#else #do
    return Math.byteswap(x)
}

// Convert a native arch integer to a big endian
#[Swag.ConstExpr, Swag.Inline]
func(T) nativeToBigEndian(x: T)->T
{
#if BIG_ENDIAN #do
    return x
#else #do
    return Math.byteswap(x)
}

// Convert a little endian integer to current arch native format
#[Swag.ConstExpr, Swag.Inline]
func(T) littleEndianToNative(x: T)->T
{
#if BIG_ENDIAN #do
    return Math.byteswap(x)
#else #do
    return x
}