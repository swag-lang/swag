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
        let readSize   = decodeData(destBuffer, DecodedBufferSize)

        decodedBufferIdx = (decodedBufferIdx + 1) % NumDecodedBuffers
        return {destBuffer, readSize}
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
            return readData(destBuffer, length)

        // Otherwise we need a codec
        /*if !codecStruct
        {
            let codec = getCodec(requestedEncoding)
            if !codec
                throw "cannot decode sound"

            codecStruct = Memory.new(codec.type)
            codecItf = @mkinterface(codecStruct, codec.type, ISoundCodec)

            streamBuffer.resize(StreamBufferSize)
            streamBuffer.count = readData(destBuffer, StreamBufferSize)
            seekInStreamBuffer = codecItf.init(streamBuffer.buffer, streamBuffer.count)
        }

        var totalOut: u64
        while totalOut < length
        {
            let used = codecItf.decode(destBuffer, length, streamBuffer.buffer, streamBuffer.count)
        }*/

        return readData(destBuffer, length)
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
        {
            stream = File.open(file.fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)
            stream.setPosition(.Begin, file.dataSeek + streamCurSeek)
        }

        // Read from file
        wantedLength = Math.min(wantedLength, file.dataSize - streamCurSeek)
        let bytesRead = stream.read(destBuffer, wantedLength)
        streamCurSeek += bytesRead
        return bytesRead
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
    mtd decode(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{read, write: u64} throw
}

struct SoundCodec
{
    type:   const *Swag.TypeInfoStruct
    itf:    ISoundCodec
}

var g_Codecs: Array'SoundCodec

// Register a codec
public func(T) addCodec()
{
    #assert Reflection.hasInterface(@typeof(T), ISoundCodec)

    var codec: SoundCodec = undefined
    codec.type = @typeof(T)
    codec.itf = @mkinterface(null, @typeof(T), ISoundCodec)
    g_Codecs.add(codec)
}

// Find the codec that can treat the given encoding
func getCodec(encoding: SoundFileEncoding)->*SoundCodec
{
    var found: *SoundCodec
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

struct CodecRaw
{
}

impl ISoundCodec for CodecRaw
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

    mtd impl decode(destBuffer: ^void, destLength: u64, srcBuffer: ^void, srcLength: u64)->{read, write: u64}  throw
    {
        return {0,0}
    }
}

// Initialize predefined codecs
#init
{
    addCodec'(CodecRaw)()
}
