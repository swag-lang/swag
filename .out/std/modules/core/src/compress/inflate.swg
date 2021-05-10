// https://github.com/odin-lang/Odin/blob/master/core/compress/zlib/zlib.odin
#global namespace compress

const ZFAST_BITS = 9
const ZFAST_MASK = (1 << ZFAST_BITS) - 1
const HUFFMAN_MAX_BITS = 16
const HUFFMAN_FAST_BITS = 9

const Z_LENGTH_BASE: [31] u16 = @[
	3, 4, 5, 6, 7, 8, 9, 10,
    11, 13, 15, 17, 19, 23, 27,
    31, 35, 43, 51, 59, 67, 83,
    99, 115, 131, 163, 195, 227, 258,
    0, 0
]

const Z_LENGTH_EXTRA: [31] u8 = @[
	0,0,0,0,0,0,0,0,
    1,1,1,1,
    2,2,2,2,
    3,3,3,3,
    4,4,4,4,
    5,5,5,5,0,0,0,
]

const Z_LENGTH_DEZIGZAG: [] u8 = @[
	16, 17, 18, 0,
    8, 7, 9, 6, 10,
    5, 11, 4, 12, 3,
    13, 2, 14, 1, 15,
]

const Z_DIST_BASE: [32] u16 = @[
	1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
	257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0,
]

const Z_DIST_EXTRA: [32] u8 = @[
	0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,0,0,
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
            maxcode[i] = cast(int) cde << (16 - i)
            cde <<=,safe 1
            k += sizes[i]
        }

        maxcode[16] = 0x10000; // Sentinel
        visit v, ci: code_lengths
        {
            if !v continue

            c := next_code[v] - firstcode[v] + firstsymbol[v]
            fastv := (cast(u16) v << 9) | cast(u16) ci
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
            s := cast(u8) (b >>,safe ZFAST_BITS)
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
        b1 := (k >>,safe (16 - s)) - ht.firstcode[s] + ht.firstsymbol[s]
        if b1 >= @countof(ht.size) throw "bad huffman code"
        if ht.size[b1] != s throw "bad huffman code"

        stream.consumeBits(cast(u8) s)
        return ht.value[b1]
    }

    func addByte(using self, dest: *Array'u8, value: u8)
    {
        dest.add(value)
        newSlice := @mkslice(dest.buffer + dest.count - 1, 1)
        adler.update(newSlice)
    }

    func parseBlock(using self, z_repeat, z_offset: *HuffmanTable, dest: *Array'u8) throw
    {
        while true
        {
            value := try decode(z_repeat)
            if value < 256
            {
                addByte(dest, cast(u8) value)
                continue
            }

            if value == 256
                return

            value -= 257
            length := Z_LENGTH_BASE[value]
            if Z_LENGTH_EXTRA[value] > 0
                length += cast(u16) stream.readBits(Z_LENGTH_EXTRA[value])

            value = try decode(z_offset)
            distance := Z_DIST_BASE[value]
            if Z_DIST_EXTRA[value] > 0
                distance += cast(u16) stream.readBits(Z_DIST_EXTRA[value])

            if dest.count < distance throw "bad distance"
            if length <= 0 continue

            if distance == 1
            {
                b := dest.buffer[dest.count - 1]
                loop length
                    addByte(dest, b)
            }
            else
            {
                loop length
                {
                    b := dest.buffer[dest.count - distance]
                    addByte(dest, b)
                }
            }
        }
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

            case 1:
                debug.assert(false) // todo
                try parseBlock(&z_repeat, &z_offset, &result)

            case 3:
                throw "invalid block type"

            default:
                var lenCodes:        [286+32+137] u8
                var codeLengthSizes: [19] u8
                stream.refill(14)
                hlit  := stream.readBitsNoRefill(5) + 257
                hdist := stream.readBitsNoRefill(5) + 1
                hclen := stream.readBitsNoRefill(4) + 4
                loop i: hclen
                {
                    s := cast(u8) stream.readBits(3)
                    codeLengthSizes[Z_LENGTH_DEZIGZAG[i]] = s
                }

                try codelength_ht.init(codeLengthSizes)
                ntot := hlit + hdist
                n := 0
                while n < ntot
                {
                    c := try decode(&codelength_ht)
                    if c < 0 or c >= 19 throw "bad code lengths"

                    if c < 16
                    {
                        lenCodes[n] = cast(u8) c
                        n += 1
                    }
                    else
                    {
                        fill := 0'u8
                        stream.refill(7)
                        switch c
                        {
                        case 16:
                            c = cast(u16) (stream.readBitsNoRefill(2) + 3)
                            if n == 0 throw "bad code lengths"
                            fill = lenCodes[n - 1]
                        case 17:
                            c = cast(u16) (stream.readBitsNoRefill(3) + 3)
                        case 18:
                            c = cast(u16) (stream.readBitsNoRefill(7) + 11)
                        default:
                            throw "bad code lengths"
                            break
                        }

                        if ntot - n < c throw "bad code lengths"
                        nc := n + c
                        while n < nc
                        {
                            lenCodes[n] = fill
                            n += 1
                        }
                    }
                }

                if n != ntot throw "bad code lengths"
                try z_repeat.init(lenCodes[0..hlit-1])
                try z_offset.init(lenCodes[hlit..ntot-1])
                try parseBlock(&z_repeat, &z_offset, &result)
            }

            if final == 1 break
        }

        return result
    }
}
