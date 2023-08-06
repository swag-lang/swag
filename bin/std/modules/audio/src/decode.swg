#global namespace Decode
using Core

func decodeData(using voice: *Voice, destBuffer: ^void, length: u64)->u64 throw
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

func rewindData(using voice: *Voice) throw
{
    streamingSeek = 0
    if stream.isOpen()
        stream.setPosition(.Begin, file.dataSeek)
}

func closeData(using voice: *Voice)
{
    streamingSeek = 0
    if stream.isOpen()
        stream.close()
}