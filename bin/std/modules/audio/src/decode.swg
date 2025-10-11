using Core, Core.Errors

impl Voice
{
    mtd readBuffer()->{ buffer: [*] void, size: u64 } throw
    {
        // If we are at the end of the file, then nothing to:, except if we need
        // to for. In that case, we rewind datas to the start
        if .streamCurSeek >= .file.dataSize
        {
            if !.playFlags.has(.Loop) do
                return {null, 0}
            .rewindData()
        }

        // If the file is prefetched and in a driver requested format, just submit the
        // prefetched datas
        if .streamCurSeek < .file.datas.count and .file.encoding == .requestedEncoding
        {
            let orgSeek  = .streamCurSeek
            let readSize = (.file.datas.count - .streamCurSeek)
            .streamCurSeek = .file.datas.count
            return {.file.datas.buffer + orgSeek, readSize}
        }

        // Otherwise we need to decode datas in a temporary buffer
        if !.decodedBuffers[.decodedBufferIdx].buffer do
            .decodedBuffers[.decodedBufferIdx].resize(DecodedBufferSize)

        let destBuffer = .decodedBuffers[.decodedBufferIdx].buffer
        let destSize   = .decodeData(destBuffer, DecodedBufferSize)

        .decodedBufferIdx = (.decodedBufferIdx + 1) % NumDecodedBuffers
        return {destBuffer, destSize}
    }

    mtd closeBuffer()
    {
        closeData(me)
    }

    mtd createCodec() throw
    {
        let desc = getCodec(.requestedEncoding, .file.encoding)
        if !desc do
            throw UnsupportedFormatError{"unsupported encoding"}
        let decoder = cast(*Codec) Memory.new(desc.type)
        decoder.srcEncoding = .file.encoding
        decoder.dstEncoding = .requestedEncoding
        decoder.type        = desc.type
        .codec              = @mkinterface(decoder, desc.type, ICodec)
    }

    mtd destroyCodec()
    {
        if .codec == null do
            return
        let decoder = cast(*Codec) @dataof(.codec)
        if decoder.type.opDrop do
            decoder.type.opDrop(decoder)
        Memory.free(decoder, cast(u64) decoder.type.sizeof)
        .codec = null
    }

    mtd decodeData(destBuffer: [*] void, length: u64)->u64 throw
    {
        // If the file contains datas that are directly requested by the driver, no need
        // to decode, we can directly read in the destination buffer
        if .file.encoding == .requestedEncoding
        {
            let bytesRead = .readData(destBuffer, length)
            .streamCurSeek += bytesRead
            return bytesRead
        }

        // Otherwise we need a codec
        if !.codec
        {
            assume .createCodec()
            .streamBuffer.reserve(StreamBufferSize)
            .streamBuffer.count = .readData(.streamBuffer.buffer, StreamBufferSize)
            .streamCurSeek      = .codec.init(.streamBuffer.buffer, .streamBuffer.count)
        }

        .streamBuffer.count = .readData(.streamBuffer.buffer, StreamBufferSize)
        let used = .codec.decode(destBuffer, length, .streamBuffer.buffer, .streamBuffer.count)
        .streamCurSeek += used.read
        return used.write
    }

    mtd readData(destBuffer: [*] void, length: u64)->u64 throw
    {
        // Out of datas
        if .streamCurSeek >= .file.dataSize do
            return 0

        var wantedLength = length

        // First take from the prefetched buffer if there's something there, instead of reading
        // from the file.
        if .streamCurSeek < .file.datas.count
        {
            let remain = Math.min(.file.datas.count - .streamCurSeek, length)
            Memory.copy(destBuffer, .file.datas.buffer + .streamCurSeek, remain)
            .streamCurSeek += remain
            if .streamCurSeek >= .file.dataSize do
                return remain
            wantedLength -= remain
            if !wantedLength do
                return remain
        }

        // Open file for streaming
        if !.stream.isOpen() do
            .stream = File.open(.file.fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)

        // Read from file
        .stream.setPosition(.Begin, .file.dataSeek + .streamCurSeek)
        wantedLength = Math.min(wantedLength, .file.dataSize - .streamCurSeek)
        return .stream.read(destBuffer, wantedLength)
    }

    mtd rewindData() throw
    {
        .streamCurSeek = 0
        if .stream.isOpen() do
            .stream.setPosition(.Begin, .file.dataSeek)
    }

    mtd closeData()
    {
        .streamCurSeek = 0
        if .stream.isOpen() do
            .stream.close()
    }
}
