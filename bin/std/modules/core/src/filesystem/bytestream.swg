using Core.Errors

// A stream for reading from or writing to a byte buffer
public struct ByteStream
{
    readBuffer:      const [..] u8     // Buffer used for reading
    writeBuffer:     *Array'u8         // Buffer used for writing
    seek:            u64               // Current read/write position
    eof:             bool              // End-of-file flag for reads
}

public impl ByteStream
{
    // Creates a byte stream for reading from a buffer
    func openRead(readBuffer: const [..] u8)->ByteStream
    {
        var result: retval
        result.readBuffer = readBuffer
        return result
    }

    // Creates a byte stream for writing to a buffer
    func openWrite(writeBuffer: *Array'u8)->ByteStream
    {
        var result: retval
        result.writeBuffer = writeBuffer
        return result
    }

    // Returns a slice of unread bytes from the current seek to the end
    mtd remainReadToSlice()->const [..] u8
    {
        return @mkslice(.readData() + .seek, .length() - .seek)
    }

    // Returns a string of unread bytes from the current seek to the end
    mtd remainReadToString()->string
    {
        return @mkstring(.readData() + .seek, .length() - .seek)
    }

    // Returns the total length of the buffer (read or write)
    mtd const length()->u64
    {
        return .writeBuffer ? .writeBuffer.count : @countof(.readBuffer)
    }

    // Returns a pointer to the underlying read or write buffer
    mtd readData()->const [*] u8
    {
        return .writeBuffer ? .writeBuffer.buffer : @dataof(.readBuffer)
    }

    // Returns true if the end of the buffer was reached during reading
    mtd const isEof()->bool
    {
        return .eof
    }

    // Writes a native value of type 'T' to the stream
    mtd(T) writeNative(val: T)
    {
        Debug.assert(writeBuffer != null)

        if seek + #sizeof(T) > writeBuffer.count do
            writeBuffer.reserve(seek + #sizeof(T))
        var ptr = cast(*T) (writeBuffer.buffer + seek)
        dref ptr = val
        seek += #sizeof(T)
        writeBuffer.count = Math.max(writeBuffer.count, seek)
    }

    // Writes a slice of bytes to the stream
    mtd writeBytes(slice: const [..] u8)
    {
        Debug.assert(.writeBuffer != null)

        let count = @countof(slice)
        if !count do
            return

        if .seek + count > .writeBuffer.count do
            .writeBuffer.reserve(.seek + count)
        let ptr = .writeBuffer.buffer + .seek
        Memory.copy(ptr, @dataof(slice), count)
        .seek += count
        .writeBuffer.count = Math.max(.writeBuffer.count, .seek)
    }

    // Reads a native value of type 'T' from the stream
    mtd(T) readNative()->T throw
    {
        if .seek + #sizeof(T) > .length() do
            throw OverflowError{"byte stream overflow"}
        let val = dref cast(const *T) (.readData() + .seek)
        .seek += #sizeof(T)
        return val
    }

    // Reads 'count' bytes from the stream into 'dest'
    mtd readBytes(dest: *u8, count: u64) throw
    {
        if .seek + count > .length() do
            throw OverflowError{"byte stream overflow"}
        Memory.copy(dest, .readData() + .seek, count)
        .seek += count
    }

    // Reads and consumes a single byte
    mtd readU8()->u8
    {
        let c = .peekU8()
        .moveSeek()
        return c
    }

    // Peeks at a single byte without consuming it
    mtd peekU8()->u8
    {
        if .seek >= .length() do
            return 0
        return .readBuffer[.seek]
    }

    // Advances the seek by a specified offset (default is 1)
    mtd moveSeek(offset: u64 = 1)
    {
        .seek += offset
        let len = .length()
        if .seek >= len
        {
            .seek = len
            .eof  = true
            return
        }
    }

    // Sets the seek to a specific offset
    mtd setSeek(offset: u64)
    {
        let len = .length()
        Debug.assert(.seek <= len)
        .seek = offset
        .eof  = .seek >= len
    }

    // Returns the current seek position
    mtd const getSeek()->u64 => .seek
}
