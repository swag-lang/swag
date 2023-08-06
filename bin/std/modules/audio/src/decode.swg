using Core

impl Voice
{
    mtd readBuffer()->{buffer: ^void, size: u64} throw
    {
        // If we are at the end of the file, then nothing to do, except if we need
        // to loop. In that case, we rewind datas to the start
        if streamingSeek >= file.dataSize
        {
             if !playFlags.has(.Loop)
                return {null, 0}
            rewindData()
        }

        // If the file is prefetched and in a driver supported format, just submit the
        // prefetched datas
        if streamingSeek < file.datas.count and Driver.supportsEncoding(file.encoding)
        {
            let readSize = (file.datas.count - streamingSeek)
            streamingSeek = file.datas.count
            return {file.datas.buffer, readSize}
        }

        // Otherwise we need to decode datas in a temporary buffer
        if !streamingBuf[curStreamBuffer].buffer
            streamingBuf[curStreamBuffer].resize(StreamingBufSize)

        let destBuffer = streamingBuf[curStreamBuffer].buffer
        let readSize   = decodeData(destBuffer, StreamingBufSize)

        curStreamBuffer = (curStreamBuffer + 1) % StreamingBufCount
        return {destBuffer, readSize}
    }

    mtd closeBuffer()
    {
        closeData(self)
    }

    mtd decodeData(destBuffer: ^void, length: u64)->u64 throw
    {
        var wantedLength = length

        // First take from the file buffer if there's something there, instead of reading
        // from the file.
        if file.datas.count and streamingSeek <= file.datas.count
        {
            let remain = Math.min(file.datas.count - streamingSeek, length)
            Memory.copy(destBuffer, file.datas.buffer + streamingSeek, remain)
            streamingSeek += remain
            if streamingSeek >= file.dataSize
                return remain
            wantedLength -= remain
            if !wantedLength
                return remain
        }

        // Open file for streaming
        if !stream.isOpen()
        {
            stream = File.open(file.fullname, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)
            stream.setPosition(.Begin, file.dataSeek + streamingSeek)
        }

        // Read from file
        wantedLength = Math.min(wantedLength, file.dataSize - streamingSeek)
        let bytesRead = stream.read(destBuffer, wantedLength)
        streamingSeek += bytesRead
        return bytesRead
    }

    mtd rewindData() throw
    {
        streamingSeek = 0
        if stream.isOpen()
            stream.setPosition(.Begin, file.dataSeek)
    }

    mtd closeData()
    {
        streamingSeek = 0
        if stream.isOpen()
            stream.close()
    }
}