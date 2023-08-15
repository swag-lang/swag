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
    byteSeek:       u64
    bufferSize:     u64
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
                if buffer.count >= 3 and buffer[0] == 0xEF and buffer[1] == 0xBB and buffer[2] == 0xBF
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
        bufferSize = Math.min(stream.getSize(), 64*1024'u64)
        bufferSize = Math.max(bufferSize, 16)
        encodingType = encoding;
    }

    // Close the reader
    func close(using self)
    {
        stream.close()
    }

    // Read one line of text.
    // Returns false at the end
    #[Swag.Safety("boundcheck|overflow", false)]
    func readLine(using self, result: ref String)->bool
    {
        result.clear()

        if byteSeek == buffer.count
        {
            readBuffer()
            if buffer.count == 0
                return false
        }

        loop
        {
            var ptr = buffer.buffer + byteSeek
            let ptrB = ptr
            let ptrE = buffer.buffer + buffer.count

            // u64 by u64
            while ptr + 8'u64 <= ptrE
            {
                let c64 = dref cast(*u64) ptr
                if Math.hasByte(c64, `\r`'u8) or Math.hasByte(c64, `\n`'u8)
                    break
                ptr += 8
            }

            // u8 by u8
            while ptr != ptrE and dref ptr != `\r`'u8 and dref ptr != `\n`'u8
                ptr += 1

            let range = cast(u64) (ptr - ptrB)
            result += @mkstring(ptrB, range)

            // We have reached the end of the buffer without finding an eol
            // Restart
            if ptr == ptrE
            {
                readBuffer()
                if buffer.count == 0
                    return true
                continue
            }

            // + 1 because we eat the eol
            byteSeek += range + 1

            // If we have an '\r', then eat the following '\n' if it exists
            if dref ptr == `\r`'u8
            {
                if byteSeek == buffer.count
                    readBuffer()
                if buffer.count and buffer[byteSeek] == `\n`'u8
                    byteSeek += 1
            }

            break
        }

        return true
    }

    // Read from the current position to the end, and returns the corresponding
    // utf8 string
    func readToEnd(using self)->String
    {
        var result: retval

        // Append what remains valid in the current decoded buffer
        if byteSeek < buffer.count
        {
            let remain = buffer.count - byteSeek
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