#global namespace File

// Text encoding formats supported for reading text files
public enum TextEncoding
{
    Ascii     // ASCII encoding
    Utf8      // UTF-8 encoding
}

// TextReader reads a text file from a FileStream with buffering and encoding support
public struct TextReader
{
    stream:            FileStream              // File stream used to read from disk
    buffer:            Array'u8                // Internal byte buffer used for reading data
    encodingType       = TextEncoding.Utf8     // Text encoding type (default is UTF-8)
    byteSeek:          u64                     // Current byte offset in the buffer
    bufferSize:        u64                     // Size of the buffer
    checkPreamble:     bool                    // Indicates whether to check for a UTF-8 BOM on first read
}

impl TextReader
{
    mtd readBuffer()
    {
        me.byteSeek = 0

        // First init. Fill force 4 bytes of the decode buffer to be 0 to
        // read the BOM
        if me.checkPreamble
        {
            me.checkPreamble = false
            me.buffer.reserve(me.bufferSize)

            if me.encodingType == .Utf8
            {
                dref cast(*u32) me.buffer.buffer = 0
                me.buffer.count                  = me.stream.read(me.buffer.buffer, me.buffer.capacity)

                // BOM
                if me.buffer.count >= 3 and me.buffer[0] == 0xEF and me.buffer[1] == 0xBB and me.buffer[2] == 0xBF:
                    me.byteSeek = 3

                return
            }
        }

        me.buffer.count = me.stream.read(me.buffer.buffer, me.buffer.capacity)
    }
}

public impl TextReader
{
    // Opens the reader with the specified text encoding (default is UTF-8)
    mtd open(fullname: string, encoding = TextEncoding.Utf8) throw
    {
        Debug.assert(!me.stream.isOpen())

        me.stream = try openRead(fullname)
        Debug.assert(me.stream.isOpen())

        me.byteSeek      = 0
        me.checkPreamble = true
        me.bufferSize    = Math.min(me.stream.getSize(), 64 * 1024'u64)
        me.bufferSize    = Math.max(me.bufferSize, 16)
        me.encodingType  = encoding
    }

    // Closes the reader and the associated stream
    mtd close()
    {
        me.stream.close()
    }

    // Reads one line of text into the result string
    // Returns false if the end of the stream has been reached
    #[Swag.Safety("boundcheck|overflow", false)]
    mtd readLine(result: &String)->bool
    {
        result.clear()

        if me.byteSeek == me.buffer.count
        {
            me.readBuffer()
            if me.buffer.count == 0:
                return false
        }

        for
        {
            var ptr  = me.buffer.buffer + me.byteSeek
            let ptrB = ptr
            let ptrE = me.buffer.buffer + me.buffer.count

            // u64 by u64
            while ptr + 8'u64 <= ptrE
            {
                let c64 = dref cast(*u64) ptr
                if Math.hasByte(c64, '\r') or Math.hasByte(c64, '\n'):
                    break
                ptr += 8
            }

            // u8 by u8
            while ptr != ptrE and dref ptr != '\r' and dref ptr != '\n':
                ptr += 1

            let range = cast(u64) (ptr - ptrB)
            result += @mkstring(ptrB, range)

            // We have reached the end of the buffer without finding an eol
            // Restart
            if ptr == ptrE
            {
                me.readBuffer()
                if me.buffer.count == 0:
                    return true
                continue
            }

            // + 1 because we eat the eol
            me.byteSeek += range + 1

            // If we have a '\r', then eat the following '\n' if it exists
            if dref ptr == '\r'
            {
                if me.byteSeek == me.buffer.count:
                    me.readBuffer()
                if me.buffer.count and me.buffer[me.byteSeek] == '\n':
                    me.byteSeek += 1
            }

            break
        }

        return true
    }

    // Reads from the current position to the end of the stream
    // and returns the content as a UTF-8 string
    mtd readToEnd()->String
    {
        var result: retval

        // Append what remains valid in the current decoded buffer
        if me.byteSeek < me.buffer.count
        {
            let remain = me.buffer.count - me.byteSeek
            result.grow(result.length + remain)
            Memory.copy(result.buffer + result.length, me.buffer.buffer + me.byteSeek, remain)
            result.length += remain
        }

        // Then read and append all the rest
        me.readBuffer()
        while me.buffer.count
        {
            result.grow(result.length + (me.buffer.count - me.byteSeek))
            Memory.copy(result.buffer + result.length, me.buffer.buffer + me.byteSeek, me.buffer.count - me.byteSeek)
            result.length += me.buffer.count
            me.readBuffer()
        }

        return result
    }
}
