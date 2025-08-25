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
        me.buffer.clear()
    }

    // Returns the number of characters
    mtd const count()->u64
    {
        return me.buffer.count()
    }

    // Set sizes of buckets of the string builder
    mtd setBucketSize(size: u64)
    {
        me.buffer.setBucketSize(size)
    }

    // Append a string
    mtd appendString(value: #null string)
    {
        me.buffer.addBytes(value)
    }

    // Append a end of line
    mtd appendEOL()
    {
        me.buffer.addBytes(Env.EndLine)
    }

    // Append a rune
    mtd appendRune(value: rune)
    {
        convertRune(&me.buffer, value)
    }

    // Append a value
    mtd appendAny(value: any, strFormat: string = null)
    {
        var convFmt: StrConv.ConvertFormat
        convertAny(&me.buffer, value, convFmt, strFormat)
    }

    // Append a formatted string
    mtd appendFormat(fmt: string, values: ...)
    {
        Format.append(&me.buffer, fmt, values)
    }

    // Force a ending 0
    mtd zeroTerminate()
    {
        me.buffer.addNative(0'u8)
    }

    // Return the content as a string
    mtd const toString()->String
    {
        return me.buffer.toString()
    }

    // Return the content as a string by eating the content of the string builder (if possible)
    mtd const moveToString()->String
    {
        return me.buffer.moveToString()
    }
}
