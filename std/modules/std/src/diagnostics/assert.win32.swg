#if std.WINDOWS && std.ASSERT
{
    using win32, std.text, std.collections

    namespace std.diagnostics.debug
    {
        // Generate an assert dialog box
        public func assertMessage(message: string, loc: swag.CompilerSourceLocation)
        {
            var str: String

            // During tests, and if not in devmode, then just raise an error without
            // a dialog box
            contextFlags := @getcontext().flags
            if((contextFlags & swag.ContextFlags.Test) && !(contextFlags & swag.ContextFlags.DevMode))
            {
                str = format("error: %:%: assertion failed", loc.fileName, loc.lineStart)
                if message != null
                {
                    str += ", "
                    str += message
                }

                @error(loc, cast(string) str)
            }

            // Build message
            if message != null
            {
                str += message
                str += "\n\n"
            }

            // Source location
            str += loc.fileName
            str += ", "
            str += cast(string) format("line %\n\n", loc.lineStart + 1)

            const info =
            #"- Press Cancel to exit
              - Press Retry to debug the application
              - Press Continue to ignore the assert
              "#
            str += info

            // Convert message to utf16 for windows
            messageW := str.toUnicode16()

            // Message box title
            str = "Swag Assertion Failed!"
            titleW := str.toUnicode16()

            result := MessageBoxW(null, messageW.buffer, titleW.buffer, MB_ICONERROR | MB_CANCELTRYCONTINUE)
            switch result
            {
                case IDCANCEL:
                    ExitProcess(0xFFFFFFFF)
                case IDTRYAGAIN:
                    DebugBreak();
                case IDCONTINUE:
                    break
            }
        }
    }
}