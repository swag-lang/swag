#if #os == "windows";
public namespace thread;
using win32

// Sleep the current thread
func sleep(ms: u32)
{
    Sleep(ms)
}