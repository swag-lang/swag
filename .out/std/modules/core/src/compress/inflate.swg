// https://github.com/odin-lang/Odin/blob/master/core/compress/zlib/zlib.odin
#global namespace compress

const ZFAST_BITS = 9
const HUFFMAN_MAX_BITS = 16

struct HuffmanTable
{
	fast:        [1 << ZFAST_BITS] u16
	firstcode:   [16] u16
	maxcode:     [17] int
	firstsymbol: [16] u16
	size:        [288] u8
	value:       [288] u16
}

impl HuffmanTable
{
    #[swag.safety("of", false)]
    func init(using self, code_lengths: const [..] u8) throw
    {
        var sizes:     [HUFFMAN_MAX_BITS + 1] u16
        var next_code: [HUFFMAN_MAX_BITS] u16

        visit v: code_lengths
            sizes[v] += 1
        sizes[0] = 0

        loop i: 1..16
        {
            if sizes[i] > (1 << i)
                throw "bad huffman sizes"
        }

	    k, cde := 0'u16
        loop i: 1..15
        {
            next_code[i]   = cde
            firstcode[i]   = cde
            firstsymbol[i] = k
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
            if !v continue

            c := next_code[v] - firstcode[v] + firstsymbol[v]
            fastv := (v << 9) | cast(u8) ci
            size[c] = v
            value[c] = cast(u16) ci
            if v <= ZFAST_BITS
            {
                j := math.reverse(next_code[v])
                j >>= 16 - v
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

struct CompressStream
{
    stream:      const [..] u8
    lenStream:   uint
    curByte:     uint
    code_buffer: u32
    num_bits:    s8
    eof:         bool
}

impl CompressStream
{
    func init(using self, buf: const [..] u8)
    {
        @init(self)
        stream = buf
        lenStream = @countof(buf)
    }

    func refill_lsb(using self, width = 24'u8)
    {
        if num_bits > width
            return
		if code_buffer == 0 and num_bits == -1
            num_bits = 0

        if code_buffer >= 1 << num_bits
        {
			num_bits = -100
            return
        }

        if curByte >= lenStream
        {
            num_bits = -42
            eof = true
            return
        }

        c := stream[curByte]
        curByte += 1

        code_buffer |= c << num_bits
		num_bits += 8
    }

    func peek_bits_lsb(using self, width: u8) -> u32
    {
	    if num_bits < width refill_lsb()
    	return code_buffer & ~(0xffffffff <<,safe width)
    }

    func read_bits_lsb(using self, width: u8)->u32
    {
        k := peek_bits_lsb(width)
        consume_bits_lsb(width)
        return k
    }

    func consume_bits_lsb(using self, width: u8)
    {
	    code_buffer >>=,safe width
	    num_bits -= width
    }
}

const DEFLATE_MAX_DISTANCE = 32768

struct HuffmanDecoder
{
    stream:         CompressStream
    window_size:    uint
    last_b:         Array'u8
}

impl HuffmanDecoder
{
    func init(using self, buf: const [..] u8, winsize: uint)
    {
        stream.init(buf)
        window_size = winsize
    }

    func inflate(using self, dest: *ConcatBuffer) throw
    {
        var z_repeat:       HuffmanTable
        var z_offset:       HuffmanTable
        var codelength_ht:  HuffmanTable

        if window_size == 0
            window_size = DEFLATE_MAX_DISTANCE

        last_b.resize(window_size)
        //while true
        {
            final := stream.read_bits_lsb(1)
            type  := stream.read_bits_lsb(2)

            switch type
            {
            case 0: break
            case 3: break
            }
        }
    }
}
