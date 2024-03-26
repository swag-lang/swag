#global public
#global namespace Math
using Swag

#[ConstExpr, Inline]
func(T) clamp(x, low, up: T)->T => @max(@min(x, up), low)

#[ConstExpr, Inline, Overload]
{
    func(T) min(x, y: T)->T       => @min(x, y)
    func(T) max(x, y: T)->T       => @max(x, y)
    func(T) min(x, y, z: T)->T    => @min(@min(x, y), z)
    func(T) max(x, y, z: T)->T    => @max(@max(x, y), z)
    func(T) min(x, y, z, w: T)->T => @min(@min(@min(x, y), z), w)
    func(T) max(x, y, z, w: T)->T => @max(@max(@max(x, y), z), w)
}

// Convert a big endian integer to current arch native format
#[ConstExpr, Inline]
func(T) bigEndianToNative(x: T)->T
{
    #if BIG_ENDIAN #do
        return x
    #else #do
        return Math.byteswap(x)
}

// Convert a native arch integer to a big endian
#[ConstExpr, Inline]
func(T) nativeToBigEndian(x: T)->T
{
    #if BIG_ENDIAN #do
        return x
    #else #do
        return Math.byteswap(x)
}

// Convert a little endian integer to current arch native format
#[ConstExpr, Inline]
func(T) littleEndianToNative(x: T)->T
{
    #if BIG_ENDIAN #do
        return Math.byteswap(x)
    #else #do
        return x
}
