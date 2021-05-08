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
    func init(using self, buffer: const [..] u8)
    {
        @init(self)
        self.buffer = #scopefct.buffer
    }

    // Returns a slice of the remaing bytes to read
    func toSlice(using self) -> const [..] u8
    {
        return @mkslice(@dataof(buffer) + seek, @countof(buffer) - seek)
    }

    // Returns length of associated slice
    func length(using const self) => @countof(buffer)

    // Returns true if end has been reached
    func isEof(using const self) => eof

    // Read one byte
    func readU8(using self)->u8
    {
        if seek >= @countof(buffer)
        {
            eof = true
            return 0
        }

        c := buffer[seek]
        seek += 1
        return c
    }
}