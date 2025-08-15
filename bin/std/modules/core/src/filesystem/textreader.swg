#global marked
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
    stream:            FileStream              // File self.stream used to read from disk
    buffer:            Array'u8                // Internal byte self.buffer used for reading data
    encodingType       = TextEncoding.Utf8     // Text encoding type (default is UTF-8)
    byteSeek:          u64                     // Current byte offset in the self.buffer
    bufferSize:        u64                     // Size of the self.buffer
    checkPreamble:     bool                    // Indicates whether to check for a UTF-8 BOM on first read
}

impl TextReader
{
    mtd readBuffer()
    {
        self.byteSeek = 0

        // First init. Fill force 4 bytes of the decode self.buffer to be 0 to
        // read the BOM
        if self.checkPreamble
        {
            self.checkPreamble = false
            self.buffer.reserve(self.bufferSize)

            if self.encodingType == .Utf8
            {
                dref cast(*u32) self.buffer.buffer = 0
                self.buffer.count                  = self.stream.read(self.buffer.buffer, self.buffer.capacity)

                // BOM
                if self.buffer.count >= 3 and self.buffer[0] == 0xEF and self.buffer[1] == 0xBB and self.buffer[2] == 0xBF:
                    self.byteSeek = 3

                return
            }
        }

        self.buffer.count = self.stream.read(self.buffer.buffer, self.buffer.capacity)
    }
}

public impl TextReader
{
    // Opens the reader with the specified text encoding (default is UTF-8)
    mtd open(fullname: string, encoding = TextEncoding.Utf8) throw
    {
        Debug.assert(!self.stream.isOpen())

        self.stream = try openRead(fullname)
        Debug.assert(self.stream.isOpen())

        self.byteSeek      = 0
        self.checkPreamble = true
        self.bufferSize    = Math.min(self.stream.getSize(), 64 * 1024'u64)
        self.bufferSize    = Math.max(self.bufferSize, 16)
        self.encodingType  = encoding
    }

    // Closes the reader and the associated self.stream
    mtd close()
    {
        self.stream.close()
    }

    // Reads one line of text into the result string
    // Returns false if the end of the self.stream has been reached
    #[Swag.Safety("boundcheck|overflow", false)]
    mtd readLine(result: &String)->bool
    {
        result.clear()

        if self.byteSeek == self.buffer.count
        {
            self.readBuffer()
            if self.buffer.count == 0:
                return false
        }

        for
        {
            var ptr  = self.buffer.buffer + self.byteSeek
            let ptrB = ptr
            let ptrE = self.buffer.buffer + self.buffer.count

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

            // We have reached the end of the self.buffer without finding an eol
            // Restart
            if ptr == ptrE
            {
                self.readBuffer()
                if self.buffer.count == 0:
                    return true
                continue
            }

            // + 1 because we eat the eol
            self.byteSeek += range + 1

            // If we have a '\r', then eat the following '\n' if it exists
            if dref ptr == '\r'
            {
                if self.byteSeek == self.buffer.count:
                    self.readBuffer()
                if self.buffer.count and self.buffer[self.byteSeek] == '\n':
                    self.byteSeek += 1
            }

            break
        }

        return true
    }

    // Reads from the current position to the end of the self.stream
    // and returns the content as a UTF-8 string
    mtd readToEnd()->String
    {
        var result: retval

        // Append what remains valid in the current decoded self.buffer
        if self.byteSeek < self.buffer.count
        {
            let remain = self.buffer.count - self.byteSeek
            result.grow(result.length + remain)
            Memory.copy(result.buffer + result.length, self.buffer.buffer + self.byteSeek, remain)
            result.length += remain
        }

        // Then read and append all the rest
        self.readBuffer()
        while self.buffer.count
        {
            result.grow(result.length + (self.buffer.count - self.byteSeek))
            Memory.copy(result.buffer + result.length, self.buffer.buffer + self.byteSeek, self.buffer.count - self.byteSeek)
            result.length += self.buffer.count
            self.readBuffer()
        }

        return result
    }
}
