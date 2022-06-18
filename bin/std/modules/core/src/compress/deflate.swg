/*
    :LICENCE
    This code is based on the miniz library (https://github.com/richgel999/miniz)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Compress

#[Swag.Opaque]
public struct Deflate
{
    stream: Stream
}

impl Deflate
{
    struct Stream
    {
        next_in:    const *u8
        avail_in:   u32
        total_in:   u32
        next_out:   *u8
        avail_out:  u32
        total_out:  u32
    }

    public enum CompressionLevel
    {
        MZ_NO_COMPRESSION = 0
        MZ_BEST_SPEED = 1
        MZ_BEST_COMPRESSION = 9
        MZ_UBER_COMPRESSION = 10
        MZ_DEFAULT_LEVEL = 6
        MZ_DEFAULT_COMPRESSION = -1
    }

    enum CompressionStrategy
    {
        MZ_DEFAULT_STRATEGY = 0
        MZ_FILTERED = 1
        MZ_HUFFMAN_ONLY = 2
        MZ_RLE = 3
        MZ_FIXED = 4
    }

    const MZ_DEFLATED = 8
    const MZ_DEFAULT_WINDOW_BITS = 15
}

impl Deflate
{
    mtd mz_deflateInit2(level: CompressionLevel, method: s32, window_bits: s32, mem_level: s32, strategy: CompressionStrategy)
    {
    }
}

public impl Deflate
{
    // Initialize the stream
    mtd init(src: const [..] u8, level = CompressionLevel.MZ_DEFAULT_COMPRESSION) throw
    {
        if @countof(src) > Swag.U32.Max
            throw "overflow"

        stream.next_in  = @dataof(src)
        stream.avail_in = cast(u32) @countof(src)

        mz_deflateInit2(level, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 9, .MZ_DEFAULT_STRATEGY)
    }

    // Compress the associated stream
    mtd compress()->Array'u8 throw
    {
        var result: retval

        stream.next_out  = result.buffer
        stream.avail_out = cast(u32) result.capacity

        return result
    }
}
