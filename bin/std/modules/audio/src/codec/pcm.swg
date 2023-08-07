using Core

struct CodecPcm
{
    using base: Codec
}

impl ICodec for CodecPcm
{
    mtd impl canDecode(encoding: SoundFileEncoding)->bool
    {
        switch encoding
        {
        case Pcm8, Pcm16, Pcm24, Pcm32, Float32:
            return true
        }

        return false
    }

    mtd impl init(srcBuffer: ^void, srcLength: u64)->u64 throw
    {
        return 0
    }

    mtd impl decode(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{write, read: u64}  throw
    {
        var dst = cast(^s16) destBuffer
        var src = cast(^s8)  srcBuffer

        var write = 0
        loop i: srcLength
        {
            if write + 2 > destLength
                break
            dst[0] = ((cast(s16) src[0]) - 128) << 8
            dst += 1
            src += 1
            write += 2
        }

        return {write, write/2}
    }
}
