namespace std.file;

public struct TextReader
{
    stream: Stream
    buffer: Array'u8
    encodingType = Encoding.Utf8
    charSeek: u32
}

impl TextReader
{
    func readBuffer(using self)
    {
        buffer.reserve(1024)
        buffer.count = file.read(stream, buffer.buffer, buffer.capacity)
        charSeek = 0
    }
}

public impl TextReader
{
    enum Encoding
    {
        Ascii
        Utf8
        Unicode
    }

    // Open the reader
    func open(using self, fullname: string, encoding: Encoding)->bool
    {
        debug.assert(!stream.isOpen())

        stream = openRead(fullname)
        if !stream.isOpen() return false

        encodingType = encoding;
        return true
    }

    // Close the reader
    func close(using self)
    {
        debug.assert(stream.isOpen())
        file.close(&stream)
    }

    func readLine(using self)->String
    {
        var result: String

        while true
        {
            if charSeek == buffer.count
                readBuffer()
            if buffer.count == 0
                return result

            while buffer[charSeek] != "\n"'u8 && buffer[charSeek] != "\r"'u8
            {
                result += buffer[charSeek]
                charSeek += 1
                if charSeek == buffer.count
                    break
            }

            if charSeek == buffer.count
                continue

            // Eat end of line characters
            charSeek += 1
            if charSeek == buffer.count
                readBuffer()
            if buffer.count && buffer[charSeek] == "\n"'u8
                charSeek += 1

            result.ensureNotNull()
            return result
        }

        return result
    }

    // Read from the current position to the end, and returns the corresponding
    // utf8 string
    func readToEnd(using self)->String
    {
        var result: String

        // Append what remains valid in the current decoded buffer
        if charSeek < buffer.count
        {
            remain := buffer.count - charSeek
            result.grow(result.length + remain)
            memory.copy(result.buffer + result.length, buffer.buffer + charSeek, remain)
            result.length += remain
        }

        // Then read and append all the rest
        readBuffer()
        while buffer.count
        {
            result.grow(result.length + buffer.count)
            memory.copy(result.buffer + result.length, buffer.buffer, buffer.count)
            result.length += buffer.count
            readBuffer()
        }

        return result
    }
}