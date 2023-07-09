public struct ByteStream
{
    readBuffer:     const [..] u8
    writeBuffer:    *Array'u8
    seek:           u64
    eof:            bool
}

public impl ByteStream
{
    // Creates a byte stream to read from a slice
    func openRead(readBuffer: const [..] u8)->ByteStream
    {
        var result: retval
        result.readBuffer = #self.readBuffer
        return result
    }

    // Creates a byte stream to write to an array
    func openWrite(writeBuffer: *Array'u8)->ByteStream
    {
        var result: retval
        result.writeBuffer = writeBuffer
        return result
    }

    // Returns a slice of the remaing bytes to read
    mtd remainReadToSlice()->const [..] u8
    {
        return @mkslice(readData() + seek, length() - seek)
    }

    // Returns a string of the remaing bytes to read
    mtd remainReadToString()->string
    {
        return @mkstring(readData() + seek, length() - seek)
    }

    // Returns length of associated slice
    mtd const length()->u64
    {
        return writeBuffer ? writeBuffer.count : @countof(readBuffer)
    }

    // Get the buffer of datas to read
    mtd readData()->const ^u8
    {
        return writeBuffer ? writeBuffer.buffer : @dataof(readBuffer)
    }

    // Returns true if end has been reached
    mtd const isEof()->bool
    {
        return eof
    }

    // Write a 'native' type value
    mtd(T) writeNative(val: T)
    {
        Debug.assert(writeBuffer != null)

        if seek + @sizeof(T) > writeBuffer.count
            writeBuffer.reserve(seek + @sizeof(T))
        var ptr = cast(*T) (writeBuffer.buffer + seek)
        dref ptr = val
        seek += @sizeof(T)
        writeBuffer.count = Math.max(writeBuffer.count, seek)
    }

    // Write the given amount of bytes
    mtd writeBytes(slice: const [..] u8)
    {
        Debug.assert(writeBuffer != null)

        let count = @countof(slice)
        if !count
            return

        if seek + count > writeBuffer.count
            writeBuffer.reserve(seek + count)
        let ptr = writeBuffer.buffer + seek
        Memory.copy(ptr, @dataof(slice), count)
        seek += count
        writeBuffer.count = Math.max(writeBuffer.count, seek)
    }

    // Read a 'native' type value
    mtd(T) readNative()->T throw
    {
        if seek + @sizeof(T) > length()
            throw "byte stream overflow"
        let val = dref cast(const *T) (readData() + seek)
        seek += @sizeof(T)
        return val
    }

    // Read the given amount of bytes
    mtd readBytes(dest: *u8, count: u64) throw
    {
        if seek + count > length()
            throw "byte stream overflow"
        Memory.copy(dest, readData() + seek, count)
        seek += count
    }

    // Read one byte and seek
    mtd readU8()->u8
    {
        var c = peekU8()
        moveSeek()
        return c
    }

    // Read one byte and seek
    mtd peekU8()->u8
    {
        if seek >= length()
            return 0
        return readBuffer[seek]
    }

    // Seek to the next byte
    mtd moveSeek(offset: u64 = 1)
    {
        seek += offset
        var len = length()
        if seek >= len
        {
            seek = len
            eof = true
            return
        }
    }

    // Seek to the given position
    mtd setSeek(offset: u64)
    {
        let len = length()
        Debug.assert(seek <= len)
        seek = offset
        eof = seek >= len
    }

    // Returns the seek value
    mtd const getSeek() => seek
}