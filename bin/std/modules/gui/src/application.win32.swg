#global if #os == Swag.TargetOs.Windows
using Win32

impl Application
{
    // Returns 'false' when need to quit
    func sendSystemEvents()->bool
    {
        var msg: MSG
        while PeekMessageW(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT
                return false
            TranslateMessage(&msg)
            DispatchMessageW(&msg)
        }

        return true
    }
}