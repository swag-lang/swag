// https://github.com/odin-lang/Odin/blob/master/core/compress/zlib/zlib.odin

const ZFAST_BITS = 9
const HUFFMAN_MAX_BITS = 16

struct HuffmanDecoder
{
	fast:        [1 << ZFAST_BITS] u16
	firstcode:   [16] u16
	maxcode:     [17] int
	firstsymbol: [16] u16
	size:        [288] u8
	value:       [288] u16
}

impl HuffmanDecoder
{
    #[swag.safety("of", false)]
    func init(using self, code_lengths: const [..] u8) throw
    {
        var sizes:     [HUFFMAN_MAX_BITS + 1] int
        var next_code: [HUFFMAN_MAX_BITS] int

        visit v: code_lengths
            sizes[v] += 1
        sizes[0] = 0

        loop i: 1..16
        {
            if sizes[i] > (1 << i)
                throw "bad huffman sizes"
        }

	    k, cde := 0'int
        loop i: 1..15
        {
            next_code[i]   = cde
            firstcode[i]   = cast(u16) cde
            firstsymbol[i] = cast(u16) k
            cde += sizes[i]
            if sizes[i] != 0 and (cde - 1) >= (1 << i)
                throw "huffman bad code lengths"
            maxcode[i] = cde << (16 - i)
            cde <<= 1
            k += sizes[i]
        }

        maxcode[16] = 0x10000; // Sentinel
        visit v, ci: code_lengths
        {
            if v != 0
            {
                c := next_code[v] - firstcode[v] + firstsymbol[v]
                fastv := (v << 9) | cast(u8) ci
                size[c]  = v
                value[c] = cast(u16) ci
                if v <= ZFAST_BITS
                {
                    j := 0//z_bit_reverse(next_code[v], v)
                    while j < (1 << ZFAST_BITS)
                    {
                        fast[j] = fastv
                        j += (1 << v)
                    }
                }

                next_code[v] += 1
            }
        }
    }
}