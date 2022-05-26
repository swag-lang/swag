#global public
#global namespace StrConv

struct StringBuilder
{
    buffer: ConcatBuffer
}

impl StringBuilder
{
    // Clear the content of the builder
    mtd clear()
    {
        buffer.clear()
    }

    // Returns the number of characters
    mtdc count()->uint
    {
        return buffer.count()
    }

    // Set sizes of buckets of the string builder
    mtd setBucketSize(size: uint)
    {
        buffer.setBucketSize(size)
    }

    // Append a string
    mtd appendString(value: string)
    {
        buffer.addBytes(value)
    }

    // Append a end of line
    mtd appendEOL()
    {
        buffer.addBytes(Env.EndLine)
    }

    // Append a rune
    mtd appendRune(value: rune)
    {
        convertRune(&buffer, value, null)
    }

    // Append a value
    mtd appendAny(value: any, strFormat: string = null)
    {
        convertAny(&buffer, value, strFormat)
    }

    // Append a formatted string
    mtd appendFormat(fmt: string, values: ...)
    {
        Format.append(&buffer, fmt, values);
    }

    // Force a ending 0
    mtd zeroTerminated()
    {
        buffer.addByte(0)
    }

    // Return the content as a string
    mtdc toString()->String
    {
        return buffer.toString()
    }
}
