#global namespace File

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
            buffer.reserve(4*1024'uint)
            decode.reserve(4*1024'uint)
            dref cast(*u32) decode.buffer = 0
        }

        // In utf8 and ascii, just read to the buffer, as we do not have do decode
        elif encodingType == Ascii or encodingType == Utf8
        {
            buffer.count = stream.read(buffer.buffer, buffer.capacity)
            return
        }

        // Read raw bytes in the decode buffer
        decode.count = stream.read(decode.buffer, decode.capacity)

        // Decode the BOM
        firstRawByte := 0'u32
        if checkPreamble
        {
            // Utf8
            if decode[0] == 0xEF and decode[1] == 0xBB and decode[2] == 0xBF
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
            Memory.copy(buffer.buffer, decode.buffer + firstRawByte, cast(uint) buffer.count)
        }
    }
}

public impl TextReader
{
    // Open the reader
    func open(using self, fullname: string, encoding = TextEncoding.Utf8) throw
    {
        Debug.assert(!stream.isOpen())

        stream = try openRead(fullname)
        Debug.assert(stream.isOpen())

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
    // Returns false at the end
    #[Swag.Safety("bound", false)]
    func readLine(using self, result: ref String)->bool
    {
        result.clear()
        scope Restart
        {
            loop
            {
                if byteSeek == buffer.count
                    readBuffer()
                if buffer.count == 0
                    return false

                startIndex := byteSeek
                while buffer.count -,safe byteSeek > 8
                {
                    ptr := cast(*u64) (buffer.buffer + byteSeek)
                    c64 := dref ptr
                    if Math.hasByte(c64, "\n"'u8) or Math.hasByte(c64, "\r"'u8)
                        break
                    byteSeek += 8
                }

                c := buffer[byteSeek]
                while c != "\n"'u8 and c != "\r"'u8
                {
                    byteSeek += 1
                    if byteSeek == buffer.count
                        continue Restart
                    c = buffer[byteSeek]
                }

                result = @mkstring(buffer.buffer + startIndex, byteSeek - startIndex)

                // Eat end of line characters
                byteSeek += 1
                if byteSeek == buffer.count
                    readBuffer()
                if c == "\r"'u8 and buffer.count and buffer[byteSeek] == "\n"'u8
                    byteSeek += 1

                return true
            }
        }

        return false
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
            Memory.copy(result.buffer + result.length, buffer.buffer + byteSeek, remain)
            result.length += remain
        }

        // Then read and append all the rest
        readBuffer()
        while buffer.count
        {
            result.grow(result.length + buffer.count)
            Memory.copy(result.buffer + result.length, buffer.buffer, buffer.count)
            result.length += buffer.count
            readBuffer()
        }

        return result
    }
}