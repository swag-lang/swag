namespace std.strconv
{
    public struct StringBuilder
    {
        buf: ConcatBuffer
    }

    public impl StringBuilder
    {
        // Set sizes of buckets of the string builder
        func setBucketSize(using self, size: u32)
        {
            buf.setBucketSize(size)
        }

        // Append a string
        func append(using self, value: string)
        {
            buf.addBytes(value)
        }

        // Append a string
        func append(using self, value: String)
        {
            buf.addBytes(cast(string) value)
        }

        // Append a character
        func append(using self, value: char)
        {
        }

        // Append a formatted string
        func appendFormat(using self, fmt: string, values: ...)
        {
            format.append(&buf, fmt, values);
        }

        // Return the content as a string
        func toString(using self)->String
        {
            return buf.toString()
        }
    }
}