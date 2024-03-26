#global export
#foreignlib "ole32"
using Swag, Win32

#[Foreign("ole32")]
{
    func CoInitializeEx(pvReserved: *void, dwCoInit: DWORD)->HRESULT;
    func CoUninitialize();
}
