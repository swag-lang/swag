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
    encodingType =  TextEncoding.Utf8
    byteSeek:       uint
    bufferSize:     uint
    checkPreamble:  bool
}

impl TextReader
{
    func readBuffer(using self)
    {
        byteSeek = 0

        // First init. Fill force 4 bytes of the decode buffer to be 0 to
        // read the bom
        if checkPreamble
        {
            checkPreamble = false
            buffer.reserve(bufferSize)

            if encodingType == .Utf8
            {
                dref cast(*u32) buffer.buffer = 0
                buffer.count = stream.read(buffer.buffer, buffer.capacity)

                // BOM
                if buffer[0] == 0xEF and buffer[1] == 0xBB and buffer[2] == 0xBF
                    byteSeek = 3
                return
            }
        }

        buffer.count = stream.read(buffer.buffer, buffer.capacity)
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
        bufferSize = Math.min(stream.getSize(), 64*1024'uint)
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

        ptr64, ptr64B := cast(^u64) (buffer.buffer + byteSeek)
        ptr64E := cast(^u64) (buffer.buffer + buffer.count)

        scope Restart
        {
            loop
            {
                if byteSeek == buffer.count
                {
                    readBuffer()
                    ptr64, ptr64B = cast(^u64) (buffer.buffer + byteSeek)
                    ptr64E = cast(^u64) (buffer.buffer + buffer.count)
                }

                if buffer.count == 0
                    return false

                startIndex := byteSeek

                // u64 by u64
                while ptr64 + 8'uint < ptr64E
                {
                    c64 := dref ptr64
                    if Math.hasByte(c64, "\n"'u8) or Math.hasByte(c64, "\r"'u8)
                        break
                    ptr64 += 1
                    byteSeek += 8
                }

                // u8 by u8
                c := buffer[byteSeek]
                while c != "\n"'u8 and c != "\r"'u8
                {
                    byteSeek += 1
                    if byteSeek == buffer.count
                        continue Restart
                    c = buffer[byteSeek]
                }

                result = @mkstring(cast(^u8) ptr64B, byteSeek - startIndex)

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
            result.grow(result.length + (buffer.count - byteSeek))
            Memory.copy(result.buffer + result.length, buffer.buffer + byteSeek, buffer.count - byteSeek)
            result.length += buffer.count
            readBuffer()
        }

        return result
    }
}