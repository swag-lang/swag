/*
    :LICENCE
    This code is based on FastNoiseLite by Jordan Peck (https://github.com/Auburn/FastNoiseLite)
    See LICENCE.md for the corresponding licence.
*/

#global namespace Noise
#global #[Swag.Safety(.All, .All, false)]

const Gradients2D: [?] f32 = [
    0.130526192220052, 0.99144486137381, 0.38268343236509, 0.923879532511287, 0.608761429008721, 0.793353340291235, 0.793353340291235, 0.608761429008721,
    0.923879532511287, 0.38268343236509, 0.99144486137381, 0.130526192220051, 0.99144486137381, -0.130526192220051, 0.923879532511287, -0.38268343236509,
    0.793353340291235, -0.60876142900872, 0.608761429008721, -0.793353340291235, 0.38268343236509, -0.923879532511287, 0.130526192220052, -0.99144486137381,
    -0.130526192220052, -0.99144486137381, -0.38268343236509, -0.923879532511287, -0.608761429008721, -0.793353340291235, -0.793353340291235, -0.608761429008721,
    -0.923879532511287, -0.38268343236509, -0.99144486137381, -0.130526192220052, -0.99144486137381, 0.130526192220051, -0.923879532511287, 0.38268343236509,
    -0.793353340291235, 0.608761429008721, -0.608761429008721, 0.793353340291235, -0.38268343236509, 0.923879532511287, -0.130526192220052, 0.99144486137381,
    0.130526192220052, 0.99144486137381, 0.38268343236509, 0.923879532511287, 0.608761429008721, 0.793353340291235, 0.793353340291235, 0.608761429008721,
    0.923879532511287, 0.38268343236509, 0.99144486137381, 0.130526192220051, 0.99144486137381, -0.130526192220051, 0.923879532511287, -0.38268343236509,
    0.793353340291235, -0.60876142900872, 0.608761429008721, -0.793353340291235, 0.38268343236509, -0.923879532511287, 0.130526192220052, -0.99144486137381,
    -0.130526192220052, -0.99144486137381, -0.38268343236509, -0.923879532511287, -0.608761429008721, -0.793353340291235, -0.793353340291235, -0.608761429008721,
    -0.923879532511287, -0.38268343236509, -0.99144486137381, -0.130526192220052, -0.99144486137381, 0.130526192220051, -0.923879532511287, 0.38268343236509,
    -0.793353340291235, 0.608761429008721, -0.608761429008721, 0.793353340291235, -0.38268343236509, 0.923879532511287, -0.130526192220052, 0.99144486137381,
    0.130526192220052, 0.99144486137381, 0.38268343236509, 0.923879532511287, 0.608761429008721, 0.793353340291235, 0.793353340291235, 0.608761429008721,
    0.923879532511287, 0.38268343236509, 0.99144486137381, 0.130526192220051, 0.99144486137381, -0.130526192220051, 0.923879532511287, -0.38268343236509,
    0.793353340291235, -0.60876142900872, 0.608761429008721, -0.793353340291235, 0.38268343236509, -0.923879532511287, 0.130526192220052, -0.99144486137381,
    -0.130526192220052, -0.99144486137381, -0.38268343236509, -0.923879532511287, -0.608761429008721, -0.793353340291235, -0.793353340291235, -0.608761429008721,
    -0.923879532511287, -0.38268343236509, -0.99144486137381, -0.130526192220052, -0.99144486137381, 0.130526192220051, -0.923879532511287, 0.38268343236509,
    -0.793353340291235, 0.608761429008721, -0.608761429008721, 0.793353340291235, -0.38268343236509, 0.923879532511287, -0.130526192220052, 0.99144486137381,
    0.130526192220052, 0.99144486137381, 0.38268343236509, 0.923879532511287, 0.608761429008721, 0.793353340291235, 0.793353340291235, 0.608761429008721,
    0.923879532511287, 0.38268343236509, 0.99144486137381, 0.130526192220051, 0.99144486137381, -0.130526192220051, 0.923879532511287, -0.38268343236509,
    0.793353340291235, -0.60876142900872, 0.608761429008721, -0.793353340291235, 0.38268343236509, -0.923879532511287, 0.130526192220052, -0.99144486137381,
    -0.130526192220052, -0.99144486137381, -0.38268343236509, -0.923879532511287, -0.608761429008721, -0.793353340291235, -0.793353340291235, -0.608761429008721,
    -0.923879532511287, -0.38268343236509, -0.99144486137381, -0.130526192220052, -0.99144486137381, 0.130526192220051, -0.923879532511287, 0.38268343236509,
    -0.793353340291235, 0.608761429008721, -0.608761429008721, 0.793353340291235, -0.38268343236509, 0.923879532511287, -0.130526192220052, 0.99144486137381,
    0.130526192220052, 0.99144486137381, 0.38268343236509, 0.923879532511287, 0.608761429008721, 0.793353340291235, 0.793353340291235, 0.608761429008721,
    0.923879532511287, 0.38268343236509, 0.99144486137381, 0.130526192220051, 0.99144486137381, -0.130526192220051, 0.923879532511287, -0.38268343236509,
    0.793353340291235, -0.60876142900872, 0.608761429008721, -0.793353340291235, 0.38268343236509, -0.923879532511287, 0.130526192220052, -0.99144486137381,
    -0.130526192220052, -0.99144486137381, -0.38268343236509, -0.923879532511287, -0.608761429008721, -0.793353340291235, -0.793353340291235, -0.608761429008721,
    -0.923879532511287, -0.38268343236509, -0.99144486137381, -0.130526192220052, -0.99144486137381, 0.130526192220051, -0.923879532511287, 0.38268343236509,
    -0.793353340291235, 0.608761429008721, -0.608761429008721, 0.793353340291235, -0.38268343236509, 0.923879532511287, -0.130526192220052, 0.99144486137381,
    0.38268343236509, 0.923879532511287, 0.923879532511287, 0.38268343236509, 0.923879532511287, -0.38268343236509, 0.38268343236509, -0.923879532511287,
    -0.38268343236509, -0.923879532511287, -0.923879532511287, -0.38268343236509, -0.923879532511287, 0.38268343236509, -0.38268343236509, 0.923879532511287]

const Gradients3D: [?] f32 = [
    0, 1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0,
    1, 0, 1, 0, -1, 0, 1, 0, 1, 0, -1, 0, -1, 0, -1, 0,
    1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0, 0,
    0, 1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0,
    1, 0, 1, 0, -1, 0, 1, 0, 1, 0, -1, 0, -1, 0, -1, 0,
    1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0, 0,
    0, 1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0,
    1, 0, 1, 0, -1, 0, 1, 0, 1, 0, -1, 0, -1, 0, -1, 0,
    1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0, 0,
    0, 1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0,
    1, 0, 1, 0, -1, 0, 1, 0, 1, 0, -1, 0, -1, 0, -1, 0,
    1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0, 0,
    0, 1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0,
    1, 0, 1, 0, -1, 0, 1, 0, 1, 0, -1, 0, -1, 0, -1, 0,
    1, 1, 0, 0, -1, 1, 0, 0, 1, -1, 0, 0, -1, -1, 0, 0,
    1, 1, 0, 0, 0, -1, 1, 0, -1, 1, 0, 0, 0, -1, -1, 0]

const PrimeX = 501125321
const PrimeY = 1136930381
const PrimeZ = 1720413743

// Hashing function for 2D coordinates.
#[Swag.Overload]
func hash(seed: s32, xPrimed, yPrimed: s32)->s32
{
    var hash = seed ^ xPrimed ^ yPrimed
    hash *= 0x27d4eb2d
    return hash
}

// Hashing function for 3D coordinates.
#[Swag.Overload]
func hash(seed: s32, xPrimed, yPrimed, zPrimed: s32)->s32
{
    var hash = seed ^ xPrimed ^ yPrimed ^ zPrimed
    hash *= 0x27d4eb2d
    return hash
}

// Gradient dot product for 2D Perlin noise.
#[Swag.Overload]
func gradCoord(seed: s32, xPrimed, yPrimed: s32, xd, yd: f32)->f32
{
    var hash = hash(seed, xPrimed, yPrimed)
    hash ^= hash >> 15
    hash &= 127 << 1

    let xg = Gradients2D[hash]
    let yg = Gradients2D[hash | 1]

    return xd * xg + yd * yg
}

// Gradient dot product for 3D Perlin noise.
#[Swag.Overload]
func gradCoord(seed: s32, xPrimed, yPrimed, zPrimed: s32, xd, yd, zd: f32)->f32
{
    var hash = hash(seed, xPrimed, yPrimed, zPrimed)
    hash ^= hash >> 15
    hash &= 63 << 2

    let xg = Gradients3D[hash]
    let yg = Gradients3D[hash | 1]
    let zg = Gradients3D[hash | 2]

    return xd * xg + yd * yg + zd * zg
}

// Value coordinate generation for 2D value noise.
#[Swag.Overload]
func valCoord(seed: s32, xPrimed, yPrimed: s32)->f32
{
    var hash = hash(seed, xPrimed, yPrimed)
    hash *= hash
    hash ^= hash << 19
    return hash * (1 / 2147483648.0)
}

// Value coordinate generation for 3D value noise.
#[Swag.Overload]
func valCoord(seed: s32, xPrimed, yPrimed, zPrimed: s32)->f32
{
    var hash = hash(seed, xPrimed, yPrimed, zPrimed)
    hash *= hash
    hash ^= hash << 19
    return hash * (1 / 2147483648.0)
}

// Perlin noise in 2D.
// Result range is [-1, 1]
#[Swag.Overload]
public func perlin(x, y: f32, seed: s32 = 1337)->f32
{
    var x0 = cast(s32) Math.floor(x)
    var y0 = cast(s32) Math.floor(y)

    let xd0 = x - x0
    let yd0 = y - y0
    let xd1 = xd0 - 1
    let yd1 = yd0 - 1

    let xs = Math.interpQuintic(xd0)
    let ys = Math.interpQuintic(yd0)

    x0 *= PrimeX
    y0 *= PrimeY
    let x1 = cast(s32) (x0 + PrimeX)
    let y1 = cast(s32) (y0 + PrimeY)

    let xf0 = Math.lerp(gradCoord(seed, x0, y0, xd0, yd0), gradCoord(seed, x1, y0, xd1, yd0), xs)
    let xf1 = Math.lerp(gradCoord(seed, x0, y1, xd0, yd1), gradCoord(seed, x1, y1, xd1, yd1), xs)

    return Math.clamp(Math.lerp(xf0, xf1, ys) * 1.4247691104677813, -1.0, 1.0)
}

// Perlin noise in 3D.
// Result range is [-1, 1]
#[Swag.Overload]
public func perlin(x, y, z: f32, seed: s32 = 1337)->f32
{
    var x0 = cast(s32) Math.floor(x)
    var y0 = cast(s32) Math.floor(y)
    var z0 = cast(s32) Math.floor(z)

    let xd0 = x - x0
    let yd0 = y - y0
    let zd0 = z - z0
    let xd1 = xd0 - 1
    let yd1 = yd0 - 1
    let zd1 = zd0 - 1

    let xs = Math.interpQuintic(xd0)
    let ys = Math.interpQuintic(yd0)
    let zs = Math.interpQuintic(zd0)

    x0 *= PrimeX
    y0 *= PrimeY
    z0 *= PrimeZ
    let x1 = cast(s32) (x0 + PrimeX)
    let y1 = cast(s32) (y0 + PrimeY)
    let z1 = cast(s32) (z0 + PrimeZ)

    let xf00 = Math.lerp(gradCoord(seed, x0, y0, z0, xd0, yd0, zd0), gradCoord(seed, x1, y0, z0, xd1, yd0, zd0), xs)
    let xf10 = Math.lerp(gradCoord(seed, x0, y1, z0, xd0, yd1, zd0), gradCoord(seed, x1, y1, z0, xd1, yd1, zd0), xs)
    let xf01 = Math.lerp(gradCoord(seed, x0, y0, z1, xd0, yd0, zd1), gradCoord(seed, x1, y0, z1, xd1, yd0, zd1), xs)
    let xf11 = Math.lerp(gradCoord(seed, x0, y1, z1, xd0, yd1, zd1), gradCoord(seed, x1, y1, z1, xd1, yd1, zd1), xs)

    let yf0 = Math.lerp(xf00, xf10, ys)
    let yf1 = Math.lerp(xf01, xf11, ys)

    return Math.clamp(Math.lerp(yf0, yf1, zs) * 0.964921414852142333984375, -1.0, 1.0)
}

// Value noise in 2D.
// Result range is [-1, 1]
#[Swag.Overload]
func value(x, y: f32, seed: s32 = 1337)->f32
{
    var x0 = cast(s32) Math.floor(x)
    var y0 = cast(s32) Math.floor(y)

    let xs = Math.interpHermite(x - x0)
    let ys = Math.interpHermite(y - y0)

    x0 *= PrimeX
    y0 *= PrimeY
    let x1 = x0 + PrimeX
    let y1 = y0 + PrimeY

    let xf0 = Math.lerp(valCoord(seed, x0, y0), valCoord(seed, x1, y0), xs)
    let xf1 = Math.lerp(valCoord(seed, x0, y1), valCoord(seed, x1, y1), xs)

    return Math.clamp(Math.lerp(xf0, xf1, ys), -1.0, 1.0)
}

// Value noise in 3D.
// Result range is [-1, 1]
#[Swag.Overload]
func value(x, y, z: f32, seed: s32 = 1337)->f32
{
    var x0 = cast(s32) Math.floor(x)
    var y0 = cast(s32) Math.floor(y)
    var z0 = cast(s32) Math.floor(z)

    let xs = Math.interpHermite(x - x0)
    let ys = Math.interpHermite(y - y0)
    let zs = Math.interpHermite(z - z0)

    x0 *= PrimeX
    y0 *= PrimeY
    z0 *= PrimeZ
    let x1 = x0 + PrimeX
    let y1 = y0 + PrimeY
    let z1 = z0 + PrimeZ

    let xf00 = Math.lerp(valCoord(seed, x0, y0, z0), valCoord(seed, x1, y0, z0), xs)
    let xf10 = Math.lerp(valCoord(seed, x0, y1, z0), valCoord(seed, x1, y1, z0), xs)
    let xf01 = Math.lerp(valCoord(seed, x0, y0, z1), valCoord(seed, x1, y0, z1), xs)
    let xf11 = Math.lerp(valCoord(seed, x0, y1, z1), valCoord(seed, x1, y1, z1), xs)

    let yf0 = Math.lerp(xf00, xf10, ys)
    let yf1 = Math.lerp(xf01, xf11, ys)

    return Math.clamp(Math.lerp(yf0, yf1, zs), -1.0, 1.0)
}
