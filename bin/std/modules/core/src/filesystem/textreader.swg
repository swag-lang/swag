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
        .byteSeek = 0

        // First init. Fill force 4 bytes of the decode buffer to be 0 to
        // read the BOM
        if .checkPreamble
        {
            .checkPreamble = false
            .buffer.reserve(.bufferSize)

            if .encodingType == .Utf8
            {
                dref cast(*u32) .buffer.buffer = 0
                .buffer.count                  = .stream.read(.buffer.buffer, .buffer.capacity)

                // BOM
                if .buffer.count >= 3 and .buffer[0] == 0xEF and .buffer[1] == 0xBB and .buffer[2] == 0xBF do
                    .byteSeek = 3

                return
            }
        }

        .buffer.count = .stream.read(.buffer.buffer, .buffer.capacity)
    }
}

public impl TextReader
{
    // Opens the reader with the specified text encoding (default is UTF-8)
    mtd open(fullname: string, encoding = TextEncoding.Utf8) throw
    {
        Debug.assert(!.stream.isOpen())

        .stream = try openRead(fullname)
        Debug.assert(.stream.isOpen())

        .byteSeek      = 0
        .checkPreamble = true
        .bufferSize    = Math.min(.stream.getSize(), 64 * 1024'u64)
        .bufferSize    = Math.max(.bufferSize, 16)
        .encodingType  = encoding
    }

    // Closes the reader and the associated stream
    mtd close()
    {
        .stream.close()
    }

    // Reads one line of text into the result string
    // Returns false if the end of the stream has been reached
    #[Swag.Safety(.All, .BoundCheck|.Overflow, false)]
    mtd readLine(result: &String)->bool
    {
        result.clear()

        if .byteSeek == .buffer.count
        {
            .readBuffer()
            if .buffer.count == 0 do
                return false
        }

        for
        {
            var ptr  = .buffer.buffer + .byteSeek
            let ptrB = ptr
            let ptrE = .buffer.buffer + .buffer.count

            // u64 by u64
            while ptr + 8'u64 <= ptrE
            {
                let c64 = dref cast(*u64) ptr
                if Math.hasByte(c64, '\r') or Math.hasByte(c64, '\n') do
                    break
                ptr += 8
            }

            // u8 by u8
            while ptr != ptrE and dref ptr != '\r' and dref ptr != '\n' do
                ptr += 1

            let range = cast(u64) (ptr - ptrB)
            result += @mkstring(ptrB, range)

            // We have reached the end of the buffer without finding an eol
            // Restart
            if ptr == ptrE
            {
                .readBuffer()
                if .buffer.count == 0 do
                    return true
                continue
            }

            // + 1 because we eat the eol
            .byteSeek += range + 1

            // If we have a '\r', then eat the following '\n' if it exists
            if dref ptr == '\r'
            {
                if .byteSeek == .buffer.count do
                    .readBuffer()
                if .buffer.count and .buffer[.byteSeek] == '\n' do
                    .byteSeek += 1
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
        if .byteSeek < .buffer.count
        {
            let remain = .buffer.count - .byteSeek
            result.grow(result.length + remain)
            Memory.copy(result.buffer + result.length, .buffer.buffer + .byteSeek, remain)
            result.length += remain
        }

        // Then read and append all the rest
        .readBuffer()
        while .buffer.count
        {
            result.grow(result.length + (.buffer.count - .byteSeek))
            Memory.copy(result.buffer + result.length, .buffer.buffer + .byteSeek, .buffer.count - .byteSeek)
            result.length += .buffer.count
            .readBuffer()
        }

        return result
    }
}
