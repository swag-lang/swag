using Core, Core.Errors

impl Voice
{
    mtd readBuffer()->{ buffer: [*] void, size: u64 } throw
    {
        // If we are at the end of the file, then nothing to:, except if we need
        // to for. In that case, we rewind datas to the start
        if me.streamCurSeek >= me.file.dataSize
        {
            if !me.playFlags.has(.Loop):
                return {null, 0}
            me.rewindData()
        }

        // If the file is prefetched and in a driver requested format, just submit the
        // prefetched datas
        if me.streamCurSeek < me.file.datas.count and me.file.encoding == me.requestedEncoding
        {
            let orgSeek  = me.streamCurSeek
            let readSize = (me.file.datas.count - me.streamCurSeek)
            me.streamCurSeek = me.file.datas.count
            return {me.file.datas.buffer + orgSeek, readSize}
        }

        // Otherwise we need to decode datas in a temporary buffer
        if !me.decodedBuffers[me.decodedBufferIdx].buffer:
            me.decodedBuffers[me.decodedBufferIdx].resize(DecodedBufferSize)

        let destBuffer = me.decodedBuffers[me.decodedBufferIdx].buffer
        let destSize   = me.decodeData(destBuffer, DecodedBufferSize)

        me.decodedBufferIdx = (me.decodedBufferIdx + 1) % NumDecodedBuffers
        return {destBuffer, destSize}
    }

    mtd closeBuffer()
    {
        closeData(me)
    }

    mtd createCodec() throw
    {
        let desc = getCodec(me.requestedEncoding, me.file.encoding)
        if !desc:
            throw UnsupportedFormatError{"unsupported encoding"}
        let decoder = cast(*Codec) Memory.new(desc.type)
        decoder.srcEncoding = me.file.encoding
        decoder.dstEncoding = me.requestedEncoding
        decoder.type        = desc.type
        me.codec            = @mkinterface(decoder, desc.type, ICodec)
    }

    mtd destroyCodec()
    {
        if me.codec == null:
            return
        let decoder = cast(*Codec) @dataof(me.codec)
        if decoder.type.opDrop:
            decoder.type.opDrop(decoder)
        Memory.free(decoder, cast(u64) decoder.type.sizeof)
        me.codec = null
    }

    mtd decodeData(destBuffer: [*] void, length: u64)->u64 throw
    {
        // If the file contains datas that are directly requested by the driver, no need
        // to decode, we can directly read in the destination buffer
        if me.file.encoding == me.requestedEncoding
        {
            let bytesRead = me.readData(destBuffer, length)
            me.streamCurSeek += bytesRead
            return bytesRead
        }

        // Otherwise we need a codec
        if !me.codec
        {
            assume me.createCodec()
            me.streamBuffer.reserve(StreamBufferSize)
            me.streamBuffer.count = me.readData(me.streamBuffer.buffer, StreamBufferSize)
            me.streamCurSeek      = me.codec.init(me.streamBuffer.buffer, me.streamBuffer.count)
        }

        me.streamBuffer.count = me.readData(me.streamBuffer.buffer, StreamBufferSize)
        let used = me.codec.decode(destBuffer, length, me.streamBuffer.buffer, me.streamBuffer.count)
        me.streamCurSeek += used.read
        return used.write
    }

    mtd readData(destBuffer: [*] void, length: u64)->u64 throw
    {
        // Out of datas
        if me.streamCurSeek >= me.file.dataSize:
            return 0

        var wantedLength = length

        // First take from the prefetched buffer if there's something there, instead of reading
        // from the file.
        if me.streamCurSeek < me.file.datas.count
        {
            let remain = Math.min(me.file.datas.count - me.streamCurSeek, length)
            Memory.copy(destBuffer, me.file.datas.buffer + me.streamCurSeek, remain)
            me.streamCurSeek += remain
            if me.streamCurSeek >= me.file.dataSize:
                return remain
            wantedLength -= remain
            if !wantedLength:
                return remain
        }

        // Open file for streaming
        if !me.stream.isOpen():
            me.stream = File.open(me.file.fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)

        // Read from file
        me.stream.setPosition(.Begin, me.file.dataSeek + me.streamCurSeek)
        wantedLength = Math.min(wantedLength, me.file.dataSize - me.streamCurSeek)
        return me.stream.read(destBuffer, wantedLength)
    }

    mtd rewindData() throw
    {
        me.streamCurSeek = 0
        if me.stream.isOpen():
            me.stream.setPosition(.Begin, me.file.dataSeek)
    }

    mtd closeData()
    {
        me.streamCurSeek = 0
        if me.stream.isOpen():
            me.stream.close()
    }
}
