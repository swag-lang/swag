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
                str += "\n"
            }

            str += loc.fileName
            str += "\n"

            const info =
            #"- Press Cancel to exit
              - Press Retry to debug the application
              - Press Continue to ignore the assert
              "#

            str += info

            // Convert message to utf16 for windows
            var messageW: Array'u16
            messageW.ensureCapacity(str.length + 1)
            messageW.count = str.length + 1
            unicode16.fromUtf8(@mkslice(messageW.buffer, str.length), @mkslice(str.buffer, str.length))
            messageW[str.length] = 0

            // Message box title
            var titleW: [128] u16 = ?
            count := unicode16.fromUtf8(titleW, "Swag Assertion Failed!")
            titleW[count] = 0

            result := MessageBoxW(null, messageW.buffer, titleW, MB_ICONERROR | MB_CANCELTRYCONTINUE)
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