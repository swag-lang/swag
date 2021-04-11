// https://github.com/golang/go/blob/master/src/math/bits/bits.go

#global public
#global namespace math
using swag

#[swag.safety("of", false)]
func mul64(x, y: u64) -> {hi: u64, lo: u64}
{
	const mask32 = (1'u64 << 32) - 1
	x0 := x & mask32
	x1 := x >> 32
	y0 := y & mask32
	y1 := y >> 32
	w0 := x0 * y0
	t := (x1 * y0) + (w0 >> 32)
	w1 := t & mask32
	w2 := t >> 32
	w1 += x0 * y1

    var result: retval
	result.hi = (x1 * y1) + w2 + (w1 >> 32)
	result.lo = x * y
	return result
}
