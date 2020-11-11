public namespace std.strconv;

struct StringBuilder
{
    buffer: ConcatBuffer
}

impl StringBuilder
{
    // Clear the content of the builder
    func clear(using self)
    {
        buffer.clear()
    }

    // Returns the number of characters
    func count(using self)->u32
    {
        return buffer.count()
    }

    // Set sizes of buckets of the string builder
    func setBucketSize(using self, size: u32)
    {
        buffer.setBucketSize(size)
    }

    // Append a string
    func append(using self, value: string)
    {
        buffer.addBytes(value)
    }

    // Append a string
    func append(using self, value: String)
    {
        buffer.addBytes(cast(string) value)
    }

    // Append a character
    func append(using self, value: char)
    {
        convertChar(&buffer, value, null)
    }

    // Append a value
    func appendAny(using self, value: any, strFormat: string = null)
    {
        convertAny(&buffer, value, strFormat)
    }

    // Append a formatted string
    func appendFormat(using self, fmt: string, values: ...)
    {
        format.append(&buffer, fmt, values);
    }

    // Return the content as a string
    func toString(using self)->String
    {
        return buffer.toString()
    }

    // Move the content as a string
    func moveToString(using self)->String
    {
        return buffer.moveToString()
    }
}
