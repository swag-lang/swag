#global namespace file

public enum TextEncoding
{
    Ascii
    Utf8
}

public struct TextReader
{
    stream:         FileStream
    buffer:         Array'u8
    decode:         Array'u8
    encodingType =  TextEncoding.Utf8
    byteSeek:       uint
    checkPreamble:  bool
}

impl TextReader
{
    func readBuffer(using self)
    {
        using TextEncoding
        byteSeek = 0

        // First init. Fill force 4 bytes of the decode buffer to be 0 to
        // read the bom
        if checkPreamble
        {
            buffer.reserve(1024)
            decode.reserve(1024)
            :cast(*u32) decode.buffer = 0
        }

        // In utf8 and ascii, just read to the buffer, as we do not have do decode
        else if encodingType == Ascii || encodingType == Utf8
        {
            buffer.count = file.read(stream, buffer.buffer, buffer.capacity)
            return
        }

        // Read raw bytes in the decode buffer
        decode.count = file.read(stream, decode.buffer, decode.capacity)

        // Decode the BOM
        firstRawByte := 0'u32
        if checkPreamble
        {
            // Utf8
            if decode[0] == 0xEF && decode[1] == 0xBB && decode[2] == 0xBF
            {
                encodingType = Utf8
                firstRawByte = 3
            }

            checkPreamble = false
        }

        // Decode
        switch encodingType
        {
        case Ascii, Utf8:
            buffer.count = decode.count - firstRawByte
            memory.copy(buffer.buffer, decode.buffer + firstRawByte, cast(uint) buffer.count)
        }
    }
}

public impl TextReader
{
    // Open the reader
    func open(using self, fullname: string, encoding = TextEncoding.Utf8)
    {
        debug.assert(!stream.isOpen())

        stream = openRead(fullname)
        if @err return
        debug.assert(stream.isOpen())

        byteSeek = 0
        checkPreamble = true
        encodingType = encoding;
    }

    // Close the reader
    func close(using self)
    {
        stream.close()
    }

    // Read one line of text.
    // Returns a null string at the end
    func readLine(using self)->String
    {
        var result: retval

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
        var result: retval

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