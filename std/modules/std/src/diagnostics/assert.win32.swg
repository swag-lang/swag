#if std.WINDOWS && std.ASSERT
{
    using win32

    public namespace std.debug
    {
        // Generate an assert dialog box
        func assertMessage(message: string, loc: swag.CompilerSourceLocation)
        {
            contextFlags := @getcontext().flags
            // If we are running bytecode, then just let the runner deal with the assertion,
            // we will have more usefull information
            if(contextFlags & swag.ContextFlags.ByteCode)
                @error(loc, message)
            // During tests, and if not in devmode, then just raise an error without
            // a dialog box
            else if((contextFlags & swag.ContextFlags.Test) && !(contextFlags & swag.ContextFlags.DevMode))
                @error(loc, message)

            // Build message
            var str: String
            if message != null
            {
                str += message
                str += "\n\n"
            }

            // Source location
            str += loc.fileName
            str += ", "
            str += cast(string) format.toString("line %\n\n", loc.lineStart + 1)

            const info =
            #"- Press Cancel to exit
              - Press Retry to debug the application
              - Press Continue to ignore the assert
              "#
            str += info

            // Convert message to utf16 for windows
            messageW := str.toUnicode16()

            // Message box title
            str = "Swag Assertion Failed !"
            titleW := str.toUnicode16()

            result := MessageBoxW(null, messageW.buffer, titleW.buffer, MB_ICONERROR | MB_CANCELTRYCONTINUE)
            switch result
            {
                case IDCANCEL:
                    ExitProcess(cast(u32) -666)
                case IDTRYAGAIN:
                    DebugBreak();
                case IDCONTINUE:
                    break
            }
        }
    }
}