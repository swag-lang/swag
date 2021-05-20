// https://github.com/odin-lang/Odin/blob/master/core/compress/zlib/zlib.odin
#global namespace compress

const FastBits           = 9
const FastMask           = (1 << FastBits) - 1
const HuffmanMaxBits     = 16
const HuffmanFastBits    = 9
const DeflateMaxDistance = 32768'uint

const LengthBase: [31] u16 = @[
	3, 4, 5, 6, 7, 8, 9, 10,
    11, 13, 15, 17, 19, 23, 27,
    31, 35, 43, 51, 59, 67, 83,
    99, 115, 131, 163, 195, 227, 258,
    0, 0
]

const LengthExtra: [31] u8 = @[
	0,0,0,0,0,0,0,0,
    1,1,1,1,
    2,2,2,2,
    3,3,3,3,
    4,4,4,4,
    5,5,5,5,0,0,0,
]

const LengthDezigzag: [] u8 = @[
	16, 17, 18, 0,
    8, 7, 9, 6, 10,
    5, 11, 4, 12, 3,
    13, 2, 14, 1, 15,
]

const DistBase: [32] u16 = @[
	1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
	257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0,
]

const DistExtra: [32] u8 = @[
	0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,0,0,
]

const FixedLength: [288]u8 = @[
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,
]

const FixedDist: [32]u8 = @[
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
]

#[Swag.inline]
func reverse16(n: u16, bits: u8)->u16
{
    r := math.reverse(n)
    r >>= 16 - bits
    return r
}

struct HuffmanTable
{
	fast:        [1 << FastBits] u16
	firstCode:   [16] u16
	maxCode:     [17] int
	firstSymbol: [16] u16
	size:        [288] u8
	value:       [288] u16
}

impl HuffmanTable
{
    func init(using self, codeLengths: const [..] u8) throw
    {
        var sizes:    [HuffmanMaxBits + 1] u16
        var nextCode: [HuffmanMaxBits] u16

        @init(self)
        visit v: codeLengths
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
            nextCode[i]    = cde
            firstCode[i]   = cde
            firstSymbol[i] = k
            cde += sizes[i]
            if sizes[i] != 0 and (cde - 1) >= (1 << i)
                throw "huffman bad code lengths"
            maxCode[i] = cast(int) cde << (16 - i)
            cde <<=,safe 1
            k += sizes[i]
        }

        maxCode[16] = 0x10000;
        visit v, ci: codeLengths
        {
            if !v continue

            c := nextCode[v] - firstCode[v] + firstSymbol[v]
            fastv := (cast(u16) v << 9) | cast(u16) ci
            size[c] = v
            value[c] = cast(u16) ci
            if v <= FastBits
            {
                j := reverse16(nextCode[v], v)
                while j < (1 << FastBits)
                {
                    fast[j] = fastv
                    j += (1'u16 << v)
                }
            }

            nextCode[v] += 1
        }
    }
}

#[Swag.opaque]
public struct DeflateDecoder
{
    stream:     CompressStream
    windowSize: uint
    adler:      hash.Adler32
}

impl DeflateDecoder
{
    func decode(using self, ht: *HuffmanTable)->u16 throw
    {
        if stream.numBits < 16
        {
            if stream.numBits == bitcast(u8) -100 throw "malformed"
            stream.refill()
            if stream.eof throw "malformed"
        }

        b := ht.fast[stream.codeBuffer & FastMask]
        if b
        {
            s := cast(u8) (b >>,safe FastBits)
            stream.consumeBits(s)
            return b & 511
        }

        cde := cast(u16) stream.peekBits(16)
	    k   := reverse16(cde, 16)

        s := HuffmanFastBits + 1
        while k >= ht.maxCode[s] s += 1

        if s >= 16 throw "bad huffman code"
        b1 := (k >>,safe (16 - s)) - ht.firstCode[s] + ht.firstSymbol[s]
        if b1 >= @countof(ht.size) throw "bad huffman code"
        if ht.size[b1] != s throw "bad huffman code"

        stream.consumeBits(cast(u8) s)
        return ht.value[b1]
    }

    func parseBlock(using self, repeatHt, repeatZ: *HuffmanTable, dest: *Array'u8) throw
    {
        startBufCount := dest.count
        loop
        {
            value := try decode(repeatHt)
            if value < 256
            {
                dest.add(cast(u8) value)
                continue
            }

            // End of block. Update checksum
            if value == 256
            {
                newSlice := @mkslice(dest.buffer + startBufCount, dest.count - startBufCount)
                adler.update(newSlice)
                return
            }

            value -= 257
            length := LengthBase[value]
            if LengthExtra[value] > 0
                length += cast(u16) stream.readBits(LengthExtra[value])

            value = try decode(repeatZ)
            distance := DistBase[value]
            if DistExtra[value] > 0
                distance += cast(u16) stream.readBits(DistExtra[value])

            if dest.count < distance throw "bad distance"
            if length <= 0 continue

            dest.grow(dest.count + length)

            if distance == 1
            {
                // Repeat the last byte 'length' times
                b := dest.buffer[dest.count - 1]
                Memory.set(dest.buffer + dest.count, b, cast(uint) length)
                dest.count += length
            }
            else if distance >= length
            {
                // Copy an already decoded sequence of bytes
                var sl: const [..] u8 = @mkslice(dest.buffer + dest.count - distance, length)
                dest.add(sl)
            }
            else
            {
                // Copy an already decoded sequence of bytes, and repeat the last one
                loop length
                {
                    b := dest.buffer[dest.count - distance]
                    dest.add(b)
                }
            }
        }
    }

    // Initialize the stream
    public func init(using self, buf: const [..] u8, winSize: uint)
    {
        stream.init(buf)
        adler.init()
        windowSize = math.min(winSize, DeflateMaxDistance)
    }

    // Decompress the associated stream
    public func decompress(using self)->Array'u8 throw
    {
        var result:        retval
        var repeatHt:      HuffmanTable
        var repeatZ:       HuffmanTable
        var codeLengthHt:  HuffmanTable

        debug.assert(windowSize > 0)

        loop
        {
            final := stream.readBits(1)
            type  := stream.readBits(2)

            switch type
            {
            // Uncompressed block
            case 0:
                stream.discardToNextByte()
                uncompressedLen := cast(u16) stream.readBits(16)
                lengthCheck     := cast(u16) stream.readBits(16)
                if ~uncompressedLen != lengthCheck throw "invalid buffer, length mismatch"

                stream.revert()
                newSlice := @mkslice(stream.curPtr(), uncompressedLen)
                adler.update(newSlice)
                result.add(newSlice)
                stream.curByte += uncompressedLen

            // Fixed length block
            case 1:
                try repeatHt.init(FixedLength)
                try repeatZ.init(FixedDist)
                try parseBlock(&repeatHt, &repeatZ, &result)

            case 3:
                throw "invalid block type"

            // Dynamic length block
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
                    codeLengthSizes[LengthDezigzag[i]] = s
                }

                try codeLengthHt.init(codeLengthSizes)
                ntot := hlit + hdist
                n := 0
                while n < ntot
                {
                    c := try decode(&codeLengthHt)
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
                try repeatHt.init(lenCodes[0..hlit-1])
                try repeatZ.init(lenCodes[hlit..ntot-1])
                try parseBlock(&repeatHt, &repeatZ, &result)
            }

            if final == 1 break
        }

        return result
    }
}
