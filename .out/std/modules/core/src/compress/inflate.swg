// https://github.com/odin-lang/Odin/blob/master/core/compress/zlib/zlib.odin
#global namespace compress

const ZFAST_BITS = 9
const ZFAST_MASK = (1 << ZFAST_BITS) - 1
const HUFFMAN_MAX_BITS = 16
const HUFFMAN_FAST_BITS = 9

const Z_LENGTH_DEZIGZAG: [] u8 = @[
	16, 17, 18, 0,
    8, 7, 9, 6, 10,
    5, 11, 4, 12, 3,
    13, 2, 14, 1, 15,
]

struct HuffmanTable
{
	fast:        [1 << ZFAST_BITS] u16
	firstcode:   [16] u16
	maxcode:     [17] int
	firstsymbol: [16] u16
	size:        [288] u8
	value:       [288] u16
}

func reverse16(n: u16, bits: u8)->u16
{
    r := math.reverse(n)
    r >>= 16 - bits
    return r
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
                j := reverse16(next_code[v], v)
                while j < (1 << ZFAST_BITS)
                {
                    fast[j] = fastv
                    j += (1'u16 << v)
                }
            }

            next_code[v] += 1
        }
    }
}

const DEFLATE_MAX_DISTANCE = 32768'uint

struct HuffmanDecoder
{
    stream:     CompressStream
    windowSize: uint
    lastB:      Array'u8
    adler:      hash.Adler32
}

impl HuffmanDecoder
{
    func init(using self, buf: const [..] u8, winSize: uint)
    {
        stream.init(buf)
        windowSize = math.min(winSize, DEFLATE_MAX_DISTANCE)
    }

    func decode(using self, ht: *HuffmanTable)->u16 throw
    {
        if stream.numBits < 16
        {
            if stream.numBits == -100 throw "malformed"
            stream.refill()
            if stream.eof throw "malformed"
        }

        b := ht.fast[stream.codeBuffer & ZFAST_MASK]
        if b
        {
            s := cast(u8) (b >> ZFAST_BITS)
            stream.consumeBits(s)
            return b & 511
        }

        cde := cast(u16) stream.peekBits(16)
	    k   := reverse16(cde, 16)

        s := HUFFMAN_FAST_BITS + 1
        while true
        {
            if k < ht.maxcode[s] break
    		s += 1
        }

        if s >= 16 throw "bad huffman code"
        b1 := (k >> (16 - s)) - ht.firstcode[s] + ht.firstsymbol[s]

        return 0
    }

    func inflate(using self)->Array'u8 throw
    {
        var result:         retval
        var z_repeat:       HuffmanTable
        var z_offset:       HuffmanTable
        var codelength_ht:  HuffmanTable

        debug.assert(windowSize > 0)
        lastB.resize(windowSize)

        while true
        {
            final := stream.readBits(1)
            type  := stream.readBits(2)
            console.printf("final: % type: %\n", final, type)

            switch type
            {
            // Uncompressed block
            case 0:
                stream.discardToNextByte()
                uncompressedLen := cast(u16) stream.readBits(16)
                lengthCheck     := cast(u16) stream.readBits(16)
                console.printf("uncompressedLen: % lengthCheck: %\n", uncompressedLen, lengthCheck)
                if ~uncompressedLen != lengthCheck throw "invalid buffer, length mismatch"

                stream.revert()
                newSlice := @mkslice(stream.curPtr(), uncompressedLen)
                adler.update(newSlice)
                result.add(newSlice)
                stream.curByte += uncompressedLen
                break

            case 3:
                throw "invalid block type"
                break

            default:
				var lenCodes:        [286+32+137] u8
				var codeLengthSizes: [19] u8
				hlit  := stream.readBits(5) + 257
				hdist := stream.readBits(5) + 1
				hclen := stream.readBits(4) + 4
                loop i: hclen
                {
					s := cast(u8) stream.readBits(3)
					codeLengthSizes[Z_LENGTH_DEZIGZAG[i]] = s
				}

                try codelength_ht.init(codeLengthSizes)
				ntot  := hlit + hdist
                loop n: ntot
                {
                    c := try decode(&codelength_ht)
                }

                return result
            }

            if final == 1 break
        }

        return result
    }
}
