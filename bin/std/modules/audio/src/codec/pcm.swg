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

    mtd impl decode(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{write, read: u64} throw
    {
        func(TD, TS) do(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{write, read: u64}
        {
            var dst = cast(^TD) destBuffer
            var src = cast(^TS) srcBuffer

            var write = 0
            loop i: srcLength
            {
                if write + @sizeof(TD) > destLength
                    break
                dst[0] = ((cast(TD) src[0]) - 128) << 8
                dst += 1
                src += 1
                write += @sizeof(TD)
            }

            return {write, write/2}
        }

        return do'(s16, s8)(destBuffer, destLength, srcBuffer, srcLength)
    }
}
