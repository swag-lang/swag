#global export
#foreignlib "ole32"
using Swag, Win32

#[Foreign("ole32")]
{
    func CoInitializeEx(pvReserved: nl *void, dwCoInit: DWORD)->HRESULT;
    func CoUninitialize();
}
