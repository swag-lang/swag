/*
    :LICENCE
    This code is based on the Odin library (https://github.com/odin-lang/Odin/blob/master/core/compress/zlib/zlib.odin)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Compress
using Core.Errors

private
{

    const FastBits        = 9
    const FastMask        = (1 << FastBits) - 1
    const HuffmanMaxBits  = 16
    const HuffmanFastBits = 9

    const LengthBase: [31] u16 = [
        3, 4, 5, 6, 7, 8, 9, 10,
        11, 13, 15, 17, 19, 23, 27,
        31, 35, 43, 51, 59, 67, 83,
        99, 115, 131, 163, 195, 227, 258,
        0, 0]

    const LengthExtra: [31] u8 = [
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1,
        2, 2, 2, 2,
        3, 3, 3, 3,
        4, 4, 4, 4,
        5, 5, 5, 5, 0, 0, 0]

    const LengthDezigzag: [?] u8 = [
        16, 17, 18, 0,
        8, 7, 9, 6, 10,
        5, 11, 4, 12, 3,
        13, 2, 14, 1, 15]

    const DistBase: [32] u16 = [
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0]

    const DistExtra: [32] u8 = [
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 0, 0]

    const FixedLength: [288] u8 = [
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8]

    const FixedDist: [32] u8 = [
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5]

    #[Swag.Inline]
    func reverse16(n: u16, bits: u8)->u16
    {
        var r = Math.reverse(n)
        r >>= 16 - bits
        return r
    }

    struct HuffmanTable
    {
        fast:            [1 << FastBits] u16
        firstCode:       [16] u16
        maxCode:         [17] s64
        firstSymbol:     [16] u16
        size:            [288] u8
        value:           [288] u16
    }

    impl HuffmanTable
    {
        mtd init(codeLengths: const [..] u8) throw
        {
            var sizes:    [HuffmanMaxBits + 1] u16
            var nextCode: [HuffmanMaxBits] u16

            @init(me, 1)
            foreach v in codeLengths do
                sizes[v] += 1
            sizes[0] = 0

            for i in 1 to 16
            {
                if sizes[i] > (1 << i) do
                    throw InvalidFormatError{"bad huffman sizes"}
            }

            var k, cde = 0'u16
            for i in 1 to 15
            {
                nextCode[i]     = cde
                .firstCode[i]   = cde
                .firstSymbol[i] = k
                cde += sizes[i]
                if sizes[i] != 0 and (cde - 1) >= (1 << i) do
                    throw InvalidFormatError{"huffman bad code lengths"}
                .maxCode[i] = cast(s64) cde << (16 - i)
                cde <<= 1
                k += sizes[i]
            }

            .maxCode[16] = 0x10000
            foreach v, ci in codeLengths
            {
                if !v do
                    continue

                let c     = nextCode[v] - .firstCode[v] + .firstSymbol[v]
                let fastv = (cast(u16) v << 9) | cast(u16) ci
                .size[c]  = v
                .value[c] = cast(u16) ci
                if v <= FastBits
                {
                    var j = reverse16(nextCode[v], v)
                    while j < (1 << FastBits)
                    {
                        .fast[j] = fastv
                        j += (1'u16 << v)
                    }
                }

                nextCode[v] += 1
            }
        }
    }
}

#[Swag.Opaque]
public struct Inflate
{
    stream:         BitStream
    windowSize:     u32
    adler:          Hash.Adler32
}

impl Inflate
{
    public const InflateMaxDistance = 32768'u32

    mtd decode(ht: *HuffmanTable)->u16 throw
    {
        if .stream.numBits < 16
        {
            .stream.refill()
            if .stream.numBits == 255 do
                throw InvalidFormatError{"malformed"}
        }

        let b = ht.fast[.stream.codeBuffer & FastMask]
        if b
        {
            let s = cast(u8) (b >> FastBits)
            .stream.consumeBits(s)
            return b & 511
        }

        let cde = cast(u16) .stream.peekBits(16)
        let k   = reverse16(cde, 16)

        var s = HuffmanFastBits + 1
        while k >= ht.maxCode[s] do
            s += 1

        if s >= 16 do
            throw InvalidFormatError{"bad huffman code"}
        let b1 = (k >> (16 - s)) - ht.firstCode[s] + ht.firstSymbol[s]
        if b1 >= @countof(ht.size) do
            throw InvalidFormatError{"bad huffman code"}
        if ht.size[b1] != s do
            throw InvalidFormatError{"bad huffman code"}

        .stream.consumeBits(cast(u8) s)
        return ht.value[b1]
    }

    mtd parseBlock(repeatHt, repeatZ: *HuffmanTable, dest: *Array'u8) throw
    {
        let startBufCount = dest.count
        for
        {
            var value = try .decode(repeatHt)
            if value < 256
            {
                dest.add(cast(u8) value)
                continue
            }

            // End of block. Update checksum
            if value == 256
            {
                let newSlice = @mkslice(dest.buffer + startBufCount, dest.count - startBufCount)
                .adler.update(newSlice)
                return
            }

            value -= 257
            var length = LengthBase[value]
            if LengthExtra[value] > 0 do
                length += cast(u16) .stream.readBits(LengthExtra[value])

            value = try .decode(repeatZ)
            var distance = DistBase[value]
            if DistExtra[value] > 0 do
                distance += cast(u16) .stream.readBits(DistExtra[value])

            if dest.count < distance do
                throw InvalidFormatError{"bad distance"}
            if length <= 0 do
                continue

            dest.grow(dest.count + length)

            if distance == 1
            {
                // Repeat the last byte 'length' times
                let b = dest.buffer[dest.count - 1]
                Memory.set(dest.buffer + dest.count, b, cast(u64) length)
                dest.count += length
            }
            elif distance >= length
            {
                // Copy an already decoded sequence of bytes
                let sl: const [..] u8 = @mkslice(dest.buffer + dest.count - distance, length)
                dest.add(sl)
            }
            else
            {
                // Copy an already decoded sequence of bytes, and repeat the last one
                for length
                {
                    let b = dest.buffer[dest.count - distance]
                    dest.add(b)
                }
            }
        }
    }

    // Decompress the associated stream
    public mtd decompress(result: *Array'u8, buf: const [..] u8, winSize: u32 = InflateMaxDistance) throw
    {
        .stream.init(buf)
        .adler.init()
        .windowSize = Math.min(winSize, InflateMaxDistance)

        var repeatHt:     HuffmanTable
        var repeatZ:      HuffmanTable
        var codeLengthHt: HuffmanTable

        Debug.assert(.windowSize > 0)

        for
        {
            let final = .stream.readBits(1)
            let type  = .stream.readBits(2)

            switch type
            {
            case 0:
                .stream.discardToNextByte()
                let uncompressedLen = cast(u16) .stream.readBits(16)
                let lengthCheck     = cast(u16) .stream.readBits(16)
                if ~uncompressedLen != lengthCheck do
                    throw InvalidFormatError{"invalid buffer, length mismatch"}

                .stream.revert()
                let newSlice = @mkslice(.stream.curPtr(), uncompressedLen)
                .adler.update(newSlice)
                result.add(newSlice)
                .stream.curByte += uncompressedLen
            case 1:
                repeatHt.init(FixedLength)
                repeatZ.init(FixedDist)
                .parseBlock(&repeatHt, &repeatZ, result)

            case 3:
                throw InvalidFormatError{"invalid block type"}
            default:
                var lenCodes:        [286 + 32 + 137] u8
                var codeLengthSizes: [19] u8
                .stream.refill(14)
                let hlit  = .stream.readBitsNoRefill(5) + 257
                let hdist = .stream.readBitsNoRefill(5) + 1
                let hclen = .stream.readBitsNoRefill(4) + 4
                for i in hclen
                {
                    let s = cast(u8) .stream.readBits(3)
                    codeLengthSizes[LengthDezigzag[i]] = s
                }

                codeLengthHt.init(codeLengthSizes)
                let ntot = hlit + hdist
                var n    = 0
                while n < ntot
                {
                    var c = .decode(&codeLengthHt)
                    if c < 0 or c >= 19 do
                        throw InvalidFormatError{"bad code lengths"}

                    if c < 16
                    {
                        lenCodes[n] = cast(u8) c
                        n += 1
                    }
                    else
                    {
                        var fill = 0'u8
                        .stream.refill(7)
                        switch c
                        {
                        case 16:
                            c = cast(u16) (.stream.readBitsNoRefill(2) + 3)
                            if n == 0 do
                                throw InvalidFormatError{"bad code lengths"}
                            fill = lenCodes[n - 1]
                        case 17:
                            c = cast(u16) (.stream.readBitsNoRefill(3) + 3)
                        case 18:
                            c = cast(u16) (.stream.readBitsNoRefill(7) + 11)
                        default:
                            throw InvalidFormatError{"bad code lengths"}
                        }

                        if ntot - n < c do
                            throw InvalidFormatError{"bad code lengths"}
                        let nc = n + c
                        while n < nc
                        {
                            lenCodes[n] = fill
                            n += 1
                        }
                    }
                }

                if n != ntot do
                    throw InvalidFormatError{"bad code lengths"}
                repeatHt.init(lenCodes[0 to hlit - 1])
                repeatZ.init(lenCodes[hlit to ntot - 1])
                .parseBlock(&repeatHt, &repeatZ, result)
            }

            if final == 1 do
                break
        }
    }
}
