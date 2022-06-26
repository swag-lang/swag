public struct ByteStream
{
    buffer: const [..] u8
    seek:   uint
    eof:    bool
}

public impl ByteStream
{
    // Creates a byte stream from a slice
    func create(buffer: const [..] u8)->ByteStream
    {
        var result: retval
        result.init(buffer)
        return result
    }

    // Initialize the byte stream with the given slice
    mtd init(buffer: const [..] u8)
    {
        @init(self)
        self.buffer = #self.buffer
    }

    // Returns a slice of the remaing bytes to read
    mtd toSlice()->const [..] u8
    {
        return @mkslice(@dataof(buffer) + seek, @countof(buffer) - seek)
    }

    // Returns a string of the remaing bytes to read
    mtd toString()->string
    {
        return @mkstring(@dataof(buffer) + seek, @countof(buffer) - seek)
    }

    // Returns length of associated slice
    mtdc length() => @countof(buffer)

    // Returns true if end has been reached
    mtdc isEof() => eof

    // Read one byte and seek
    mtd readU8()->u8
    {
        c := peekU8()
        seekNext()
        return c
    }

    // Read one byte and seek
    mtd peekU8()->u8
    {
        if seek >= @countof(buffer)
            return 0
        return buffer[seek]
    }

    // Seek to the next byte
    mtd seekNext(offset: uint = 1)
    {
        seek += offset
        if seek >= @countof(buffer)
        {
            seek = @countof(buffer)
            eof = true
            return
        }
    }
}