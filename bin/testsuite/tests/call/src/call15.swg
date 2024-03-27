#global #if #os == Swag.TargetOs.Windows
using test_foreign

using Swag

#[Swag.Foreign("kernel32")]
{
    func GetModuleHandleA(lpModuleName: const *u8)->const *void;
    func GetProcAddress(hModule: const *void, lpProcName: const *u8)->const *void;
}

//#[Swag.PrintBc, Swag.Optim(false)]
#test
{
}
