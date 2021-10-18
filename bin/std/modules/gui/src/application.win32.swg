#global if #os == Swag.TargetOs.Windows
using Win32

impl Application
{
    // Returns 'false' when need to quit
    func sendSystemEvents()
    {
        var msg: MSG
        while PeekMessageW(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            TranslateMessage(&msg)
            DispatchMessageW(&msg)
        }
    }
}