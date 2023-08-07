using Core

struct CodecPcmToPcm16
{
    using base: Codec
}

impl ICodec for CodecPcmToPcm16
{
    mtd impl canEncode(encoding: SoundFileEncoding)->bool
    {
        switch encoding
        {
        case Pcm16:
            return true
        }

        return false
    }

    mtd impl canDecode(encoding: SoundFileEncoding)->bool
    {
        switch encoding
        {
        case Pcm8:
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
        func(TS, const EN: SoundFileEncoding) do(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{write, read: u64}
        {
            var dst = cast(^s16) destBuffer
            var src = cast(^TS) srcBuffer

            var write, read = 0
            loop i: srcLength
            {
                if write + @sizeof(s16) > destLength
                    break

                #if EN == .Pcm8
                    dst[0] = ((cast(s16) src[0]) - 128) << 8
                #else
                    #assert false

                dst += 1
                src += 1
                read += @sizeof(TS)
                write += @sizeof(s16)
            }

            return {write, read}
        }

        switch srcEncoding
        {
        case Pcm8:
            return do'(s8, Pcm8)(destBuffer, destLength, srcBuffer, srcLength)
        }

        throw "error"
        return {0,0}
    }
}
