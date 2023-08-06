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

        // If the file is prefetched and in a driver supported format, just submit the
        // prefetched datas
        if streamCurSeek < file.datas.count and Driver.supportsEncoding(file.encoding)
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
        // If the file contains datas that are directly supported by the driver, no need
        // to decode, we can directly read in the output buffer
        if Driver.supportsEncoding(file.encoding)
            return readData(destBuffer, length)

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