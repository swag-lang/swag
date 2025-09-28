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
        .buffer.clear()
    }

    // Returns the number of characters
    mtd const count()->u64
    {
        return .buffer.count()
    }

    // Set sizes of buckets of the string builder
    mtd setBucketSize(size: u64)
    {
        .buffer.setBucketSize(size)
    }

    // Append a string
    mtd appendString(value: #null string)
    {
        .buffer.addBytes(value)
    }

    // Append a end of line
    mtd appendEOL()
    {
        .buffer.addBytes(Env.EndLine)
    }

    // Append a rune
    mtd appendRune(value: rune)
    {
        convertRune(&.buffer, value)
    }

    // Append a value
    mtd appendAny(value: any, strFormat: string = null)
    {
        let convFmt: StrConv.ConvertFormat
        convertAny(&.buffer, value, convFmt, strFormat)
    }

    // Append a formatted string
    mtd appendFormat(fmt: string, values: ...)
    {
        Format.append(&.buffer, fmt, values)
    }

    // Force a ending 0
    mtd zeroTerminate()
    {
        .buffer.addNative(0'u8)
    }

    // Return the content as a string
    mtd const toString()->String
    {
        return .buffer.toString()
    }

    // Return the content as a string by eating the content of the string builder (if possible)
    mtd moveToString()->String
    {
        return .buffer.moveToString()
    }
}
