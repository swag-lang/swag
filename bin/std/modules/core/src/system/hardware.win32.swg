#global #if WINDOWS
#global namespace Hardware
using Win32

// Number of processors
public func getProcessorCount()->u32 => cast(u32) Env.g_SysInfo.dwNumberOfProcessors

// Max address
public func getMacAddress()->u64
{
    var info:   [16] IP_ADAPTER_INFO
    var length: DWORD = #sizeof(info)
    if GetAdaptersInfo(&info, &length) != 0 do
        return 0
    return dref cast(*u64) &info[0].address[0]
}
