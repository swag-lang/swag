#global namespace Serialization
#global namespace Write
#global public
using Swag

// Options for customizing JSON formatting.
struct JSonOptions
{
    indentLevel:     u32 = 4     // Number of spaces per indentation level.
    saveBlanks:      bool        // Whether to include indentation and line breaks.
}

// A JSON writer.
struct JSon
{
    options:           JSonOptions                                 // Formatting options.
    output:            *ConcatBuffer                               // Output buffer for generated JSON.
    indent:            u32                                         // Current indentation level.
    fmtInt:            StrConv.ConvertFormatInt                    // Format used for converting integers.
    fmtFloat:          StrConv.ConvertFormatFloat{fmt: 'g''u8}     // Format used for converting floating-point numbers.
    lastSeekValue:     ConcatBufferSeek                            // Seek position used for rollback of trailing commas.
}

impl JSon
{
    // Returns true indicating this writer produces textual output.
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd isTextual()->bool
    {
        return true
    }

    // Writes indentation based on the current indent level and settings.
    mtd writeIndent()
    {
        if !me.options.saveBlanks:
            return
        for me.indent * me.options.indentLevel:
            me.output.addNative(' ''u8)
    }

    // Initializes the JSON writer with the given output buffer.
    mtd startWrite(buf: *ConcatBuffer)
    {
        me.output = buf
    }

    // Begins the root element (no-op for JSON).
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginRoot() {}

    // Ends the root element (no-op for JSON).
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endRoot() {}

    // Begins a JSON array.
    mtd beginSequence(_typeElem: typeinfo, _data: const *void, _countElem: u64)->bool
    {
        me.output.addBytes("[")
        me.lastSeekValue = me.output.getSeek()
        if me.options.saveBlanks
        {
            me.output.addBytes("\n")
            me.indent += 1
            me.writeIndent()
        }

        return false
    }

    // Ends a JSON array.
    mtd endSequence()
    {
        me.output.setEndSeek(me.lastSeekValue)
        if me.options.saveBlanks
        {
            me.output.addBytes("\n")
            me.indent -= 1
            me.writeIndent()
        }

        me.output.addBytes("]")
        me.lastSeekValue = me.output.getSeek()
    }

    // Begins a JSON object section.
    mtd beginSection(_kind: SectionKind)
    {
        me.output.addBytes("{")
        me.lastSeekValue = me.output.getSeek()
        if me.options.saveBlanks
        {
            me.output.addBytes("\n")
            me.indent += 1
            me.writeIndent()
        }
    }

    // Ends a JSON object section.
    mtd endSection()
    {
        me.output.setEndSeek(me.lastSeekValue)
        if me.options.saveBlanks
        {
            me.output.addBytes("\n")
            me.indent -= 1
            me.writeIndent()
        }

        me.output.addBytes("}")
        me.lastSeekValue = me.output.getSeek()
    }

    // Begins writing a struct (treated as a JSON object).
    mtd beginStruct(_structType: typeinfo, _structData: const *u8)->bool
    {
        me.beginSection(.Ordered)
        return false
    }

    // Ends writing a struct.
    mtd endStruct()
    {
        me.endSection()
    }

    // Begins a named JSON element (a key-value pair).
    mtd beginElement(val: TypedValue)
    {
        me.output.addNative('"''u8)
        me.output.addBytes(val.name)
        me.output.addBytes("\":")
        if me.options.saveBlanks:
            me.output.addBytes(" ")
    }

    // Ends a named JSON element (no-op).
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endElement(val: TypedValue) {}

    // Begins a JSON value (no-op).
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginValue() {}

    // Ends a JSON value and appends a comma and optional newline.
    mtd endValue()
    {
        me.lastSeekValue = me.output.getSeek()
        me.output.addBytes(",")
        if me.options.saveBlanks:
            me.output.addBytes("\n")
        me.writeIndent()
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    // Writes a native value (bool, float, int, string) as a JSON value.
    mtd(NT) writeNative(value: NT)
    {
        #if NT == bool
        {
            output.addBytes(value ? "true" : "false")
        }
        #elif Reflection.isFloat(NT)
        {
            StrConv.convertFloat(output, value, fmtFloat)
        }
        #elif Reflection.isInteger(NT)
        {
            StrConv.convertInt(output, value, fmtInt)
        }
        #elif NT == string
        {
            output.addNative('"''u8)
            output.addBytes(value)
            output.addNative('"''u8)
        }
        #else
        {
            #error("invalid type")
        }
    }
}
