#if swag.WINDOWS && std.ASSERT
{
    using win32, std

    namespace std.diagnostics.debug
    {
        /// Generate an assert dialog box
        public func assertMessage(message: string)
        {
            // User message
            var messageW: [512] u16
            count1 := text.unicode16.fromUtf8(messageW, message)

            const info =
            #"
            - Press Cancel to exit
            - Press Retry to debug the application
            - Press Continue to ignore the assert
            "#

            count2 := text.unicode16.fromUtf8({&messageW[count1], @countof(messageW) - count1}, info)
            messageW[count1 + count2] = 0

            // Message box title
            var titleW: [128] u16 = ?
            count1 = text.unicode16.fromUtf8(titleW, "Swag Assertion Failed!")
            titleW[count1] = 0

            result := MessageBoxW(null, messageW, titleW, MB_ICONERROR | MB_CANCELTRYCONTINUE)
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