using tests_foreign

using Swag

#[Swag.Foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: *void, dwStackSize: u64, lpStartAddress: *void, lpParameter: *void, dwCreationFlags: u32, lpThreadId : *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds : u32)->u32;
}

//#[Swag.PrintBc, Swag.Optim(false)]
#test
{
    var paramBC: [2] *void
    paramBC[1] = acast 666'uint

    id := 0'u32
    h := CreateThread(null, 0, @mkcallback(&tcf_entry), cast(*void) &paramBC[0], 0, &id)
    const INFINITE = 0xFFFFFFFF
    ew := WaitForSingleObject(h, INFINITE);
}