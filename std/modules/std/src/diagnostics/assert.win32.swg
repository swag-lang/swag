#if std.WINDOWS && std.ASSERT
{
    using win32, std.text, std.collections

    namespace std.diagnostics.debug
    {
        // Generate an assert dialog box
        public func assertMessage(message: string, loc: swag.CompilerSourceLocation)
        {
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