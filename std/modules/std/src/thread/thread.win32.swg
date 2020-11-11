#if #os == "windows";
public namespace thread;
using win32

// Sleep the current thread for a given amount of milliseconds
func sleep(ms: u32)
{
    Sleep(ms)
}