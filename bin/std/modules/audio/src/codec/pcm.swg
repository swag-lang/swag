using Core, Core.Errors

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
        case Pcm8, Pcm16, Pcm32, Float32:
            return true
        }

        return false
    }

    mtd impl init(srcBuffer: [*] void, srcLength: u64)->u64 throw
    {
        return 0
    }

    mtd impl decode(destBuffer: [*] void, destLength: u64, srcBuffer: [*] void, srcLength: u64)->{ write, read: u64 } throw
    {
        if .srcEncoding == .dstEncoding
        {
            Memory.copy(destBuffer, srcBuffer, srcLength)
            return {srcLength, srcLength}
        }

        func(TS: s32, EN: SoundFileEncoding) doIt(destBuffer: [*] void, destLength: u64, srcBuffer: [*] void, srcLength: u64)->{ write, read: u64 }
        {
            var dst = cast([*] s16) destBuffer
            var src = cast([*] s8) srcBuffer

            var write, read = 0
            for i in srcLength / TS
            {
                if write + #sizeof(s16) > destLength do
                    break

                #if EN == .Pcm8
                {
                    dst[0] = (cast(s16) src[0] - 128) << 8
                }
                #elif EN == .Pcm24
                {
                    var intS: s32
                    intS = cast(s32) src[2] << 16
                    intS |= cast(s32) src[1] << 8
                    intS |= src[0]
                    intS   = Math.clamp(intS, -32768, 32767)
                    dst[0] = cast(s16) intS
                }
                #elif EN == .Pcm32
                {
                    let input = cast([*] s32) src
                    dst[0] = cast(s16) (input[0] >> 16)
                }
                #elif EN == .Float32
                {
                    let input  = cast([*] f32) src
                    let sample = input[0] * 32768.0
                    let intS   = Math.clamp(cast #wrap (s32) sample, -32768, 32767)
                    dst[0] = cast(s16) intS
                }
                #else do
                    #assert(false)

                dst += 1
                src += TS
                write += #sizeof(s16)
                read += TS
            }

            return {write, read}
        }

        switch .srcEncoding
        {
        case Pcm8:
            return doIt'(1, Pcm8)(destBuffer, destLength, srcBuffer, srcLength)
        case Pcm24:
            return doIt'(3, Pcm24)(destBuffer, destLength, srcBuffer, srcLength)
        case Pcm32:
            return doIt'(4, Pcm32)(destBuffer, destLength, srcBuffer, srcLength)
        case Float32:
            return doIt'(4, Float32)(destBuffer, destLength, srcBuffer, srcLength)
        }

        throw UnsupportedFormatError{"invalid encoding"}
        return {0, 0}
    }
}
