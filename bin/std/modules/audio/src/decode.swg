using Core

func streamingDecodeData(using voice: *Voice, destBuffer: ^u8, length: u64)->u64 throw
{
    var wantedLength = length

    // First take from the file buffer if there's something there, instead of reading
    // from the file.
    if file.datas.count and streamingSeek <= file.datas.count
    {
        var remain = file.datas.count - streamingSeek
        remain = Math.min(remain, length)
        Memory.copy(destBuffer, file.datas.buffer + streamingSeek, remain)
        streamingSeek += remain
        if streamingSeek >= file.dataSize
            return remain
        wantedLength -= remain
        if !wantedLength
            return remain
    }

    if !file.stream.isOpen()
    {
        file.stream = File.open(file.stream.name, File.FileMode.Open, File.FileAccess.Read, File.FileShare.Read)
        file.stream.setPosition(.Begin, file.dataSeek + streamingSeek)
    }

    wantedLength = Math.min(wantedLength, file.dataSize - streamingSeek)

    let result = file.stream.read(destBuffer, wantedLength)
    streamingSeek += result

    return result
}

func streamingRewindData(using voice: *Voice) throw
{
    streamingSeek = 0
    if file.stream.isOpen()
        file.stream.setPosition(.Begin, file.dataSeek)
}

func streamingCloseData(using voice: *Voice)
{
    streamingSeek = 0
    if file.stream.isOpen()
        file.stream.close()
}