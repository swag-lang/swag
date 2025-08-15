#global marked
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
        result.readBuffer = #self.readBuffer
        return result
    }

    // Creates a byte stream for writing to a buffer
    func openWrite(writeBuffer: *Array'u8)->ByteStream
    {
        var result: retval
        result.writeBuffer = writeBuffer
        return result
    }

    // Returns a slice of unread bytes from the current self.seek to the end
    mtd remainReadToSlice()->const [..] u8
    {
        return @mkslice(self.readData() + self.seek, self.length() - self.seek)
    }

    // Returns a string of unread bytes from the current self.seek to the end
    mtd remainReadToString()->string
    {
        return @mkstring(self.readData() + self.seek, self.length() - self.seek)
    }

    // Returns the total length of the buffer (read or write)
    mtd const length()->u64
    {
        return self.writeBuffer ? self.writeBuffer.count : @countof(self.readBuffer)
    }

    // Returns a pointer to the underlying read or write buffer
    mtd readData()->const ^u8
    {
        return self.writeBuffer ? self.writeBuffer.buffer : @dataof(self.readBuffer)
    }

    // Returns true if the end of the buffer was reached during reading
    mtd const isEof()->bool
    {
        return self.eof
    }

    // Writes a native value of type 'T' to the stream
    mtd(T) writeNative(val: T)
    {
        Debug.assert(writeBuffer != null)

        if self.seek + #sizeof(T) > self.writeBuffer.count:
            self.writeBuffer.reserve(seek + #sizeof(T))
        var ptr = cast(*T) (writeBuffer.buffer + self.seek)
        dref ptr = val
        self.seek += #sizeof(T)
        self.writeBuffer.count = Math.max(writeBuffer.count, self.seek)
    }

    // Writes a slice of bytes to the stream
    mtd writeBytes(slice: const [..] u8)
    {
        Debug.assert(self.writeBuffer != null)

        let count = @countof(slice)
        if !count:
            return

        if self.seek + count > self.writeBuffer.count:
            self.writeBuffer.reserve(self.seek + count)
        let ptr = self.writeBuffer.buffer + self.seek
        Memory.copy(ptr, @dataof(slice), count)
        self.seek += count
        self.writeBuffer.count = Math.max(self.writeBuffer.count, self.seek)
    }

    // Reads a native value of type 'T' from the stream
    mtd(T) readNative()->T throw
    {
        if self.seek + #sizeof(T) > self.length():
            throw OverflowError{"byte stream overflow"}
        let val = dref cast(const *T) (self.readData() + self.seek)
        self.seek += #sizeof(T)
        return val
    }

    // Reads 'count' bytes from the stream into 'dest'
    mtd readBytes(dest: *u8, count: u64) throw
    {
        if self.seek + count > self.length():
            throw OverflowError{"byte stream overflow"}
        Memory.copy(dest, self.readData() + self.seek, count)
        self.seek += count
    }

    // Reads and consumes a single byte
    mtd readU8()->u8
    {
        let c = self.peekU8()
        self.moveSeek()
        return c
    }

    // Peeks at a single byte without consuming it
    mtd peekU8()->u8
    {
        if self.seek >= self.length():
            return 0
        return self.readBuffer[self.seek]
    }

    // Advances the self.seek by a specified offset (default is 1)
    mtd moveSeek(offset: u64 = 1)
    {
        self.seek += offset
        let len = self.length()
        if self.seek >= len
        {
            self.seek = len
            self.eof  = true
            return
        }
    }

    // Sets the self.seek to a specific offset
    mtd setSeek(offset: u64)
    {
        let len = self.length()
        Debug.assert(self.seek <= len)
        self.seek = offset
        self.eof  = self.seek >= len
    }

    // Returns the current self.seek position
    mtd const getSeek()->u64 => self.seek
}
