using Core

impl Voice
{
    mtd readBuffer()->{buffer: ^void, size: u64} throw
    {
        // If we are at the end of the file, then nothing to do, except if we need
        // to loop. In that case, we rewind datas to the start
        if streamCurSeek >= file.dataSize
        {
             if !playFlags.has(.Loop)
                return {null, 0}
            rewindData()
        }

        // If the file is prefetched and in a driver requested format, just submit the
        // prefetched datas
        if streamCurSeek < file.datas.count and file.encoding == requestedEncoding
        {
            let orgSeek = streamCurSeek
            let readSize = (file.datas.count - streamCurSeek)
            streamCurSeek = file.datas.count
            return {file.datas.buffer + orgSeek, readSize}
        }

        // Otherwise we need to decode datas in a temporary buffer
        if !decodedBuffers[decodedBufferIdx].buffer
            decodedBuffers[decodedBufferIdx].resize(DecodedBufferSize)

        let destBuffer = decodedBuffers[decodedBufferIdx].buffer
        let destSize   = decodeData(destBuffer, DecodedBufferSize)

        decodedBufferIdx = (decodedBufferIdx + 1) % NumDecodedBuffers
        return {destBuffer, destSize}
    }

    mtd closeBuffer()
    {
        closeData(self)
    }

    mtd decodeData(destBuffer: ^void, length: u64)->u64 throw
    {
        // If the file contains datas that are directly requested by the driver, no need
        // to decode, we can directly read in the destination buffer
        if file.encoding == requestedEncoding
        {
            let bytesRead = readData(destBuffer, length)
            streamCurSeek += bytesRead
            return bytesRead
        }

        // Otherwise we need a codec
        if !codecStruct
        {
            let codec = getCodec(requestedEncoding)
            if !codec
                throw "cannot decode sound"

            codecStruct = cast(*SoundCodec) Memory.new(codec.type)
            codecStruct.srcEncoding = file.encoding
            codecStruct.dstEncoding = requestedEncoding
            codecItf = @mkinterface(codecStruct, codec.type, ISoundCodec)

            streamBuffer.reserve(StreamBufferSize)
            streamBuffer.count = readData(streamBuffer.buffer, StreamBufferSize)
            streamCurSeek = codecItf.init(streamBuffer.buffer, streamBuffer.count)
        }

        streamBuffer.count = readData(streamBuffer.buffer, StreamBufferSize)
        let used = codecItf.decode(destBuffer, length, streamBuffer.buffer, streamBuffer.count)
        streamCurSeek += used.read
        return used.write
    }

    mtd readData(destBuffer: ^void, length: u64)->u64 throw
    {
        // Out of datas
        if streamCurSeek >= file.dataSize
            return 0

        var wantedLength = length

        // First take from the prefetched buffer if there's something there, instead of reading
        // from the file.
        if streamCurSeek < file.datas.count
        {
            let remain = Math.min(file.datas.count - streamCurSeek, length)
            Memory.copy(destBuffer, file.datas.buffer + streamCurSeek, remain)
            streamCurSeek += remain
            if streamCurSeek >= file.dataSize
                return remain
            wantedLength -= remain
            if !wantedLength
                return remain
        }

        // Open file for streaming
        if !stream.isOpen()
            stream = File.open(file.fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)

        // Read from file
        stream.setPosition(.Begin, file.dataSeek + streamCurSeek)
        wantedLength = Math.min(wantedLength, file.dataSize - streamCurSeek)
        return stream.read(destBuffer, wantedLength)
    }

    mtd rewindData() throw
    {
        streamCurSeek = 0
        if stream.isOpen()
            stream.setPosition(.Begin, file.dataSeek)
    }

    mtd closeData()
    {
        streamCurSeek = 0
        if stream.isOpen()
            stream.close()
    }
}

public interface ISoundCodec
{
    mtd canDecode(encoding: SoundFileEncoding)->bool
    mtd init(srcBuffer: ^void, srcLength: u64)->u64 throw
    mtd decode(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{write, read: u64} throw
}

struct SoundCodecDesc
{
    type:   const *Swag.TypeInfoStruct
    itf:    ISoundCodec
}

var g_Codecs: Array'SoundCodecDesc

// Register a codec
public func(T) addCodec()
{
    #assert Reflection.hasInterface(@typeof(T), ISoundCodec)

    var codec: SoundCodecDesc = undefined
    codec.type = @typeof(T)
    codec.itf = @mkinterface(null, @typeof(T), ISoundCodec)
    g_Codecs.add(codec)
}

// Find the codec that can treat the given encoding
func getCodec(encoding: SoundFileEncoding)->*SoundCodecDesc
{
    var found: *SoundCodecDesc
    visit &codec : g_Codecs
    {
        if codec.itf.canDecode(encoding)
        {
            found = codec
            break
        }
    }

    return found
}

struct SoundCodec
{
    srcEncoding: SoundFileEncoding
    dstEncoding: SoundFileEncoding
}

struct SoundCodecRaw
{
    using base: SoundCodec
}

impl ISoundCodec for SoundCodecRaw
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

// Initialize predefined codecs
#init
{
    addCodec'(SoundCodecRaw)()
}
