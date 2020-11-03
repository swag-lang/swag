namespace std.file;

public struct TextReader
{
    stream:         Stream
    buffer:         Array'u8
    encodingType =  Encoding.Utf8
    byteSeek:       u32
}

impl TextReader
{
    func readBuffer(using self)
    {
        buffer.reserve(1024)
        buffer.count = file.read(stream, buffer.buffer, buffer.capacity)
        byteSeek = 0
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
    func open(using self, fullname: string, encoding = Encoding.Utf8)->bool
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
        stream.close()
    }

    // Read one line of text.
    // Returns a null string at the end
    func readLine(using self)->String
    {
        var result: String

        while true
        {
            if byteSeek == buffer.count
                readBuffer()
            if buffer.count == 0
                return result

            while buffer[byteSeek] != "\n"'u8 && buffer[byteSeek] != "\r"'u8
            {
                result += buffer[byteSeek]
                byteSeek += 1
                if byteSeek == buffer.count
                    break
            }

            if byteSeek == buffer.count
                continue

            // Eat end of line characters
            byteSeek += 1
            if byteSeek == buffer.count
                readBuffer()
            if buffer.count && buffer[byteSeek] == "\n"'u8
                byteSeek += 1

            // Result must be empty, not null, to differenciate an empty line
            // and the end of the file
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
        if byteSeek < buffer.count
        {
            remain := buffer.count - byteSeek
            result.grow(result.length + remain)
            memory.copy(result.buffer + result.length, buffer.buffer + byteSeek, remain)
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