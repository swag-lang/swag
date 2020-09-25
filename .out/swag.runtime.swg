////////////////////////////////////////////////////////////
func @memcmp(dst, src: const *void, size: u32)->s32
{
    if !size
        return 0

    p1 := cast(const *s8) dst
    p2 := cast(const *s8) src
    loop i: size
    {
        if p1[i] != p2[i]
            return p1[i] - p2[i]
    }

    return 0
}

////////////////////////////////////////////////////////////
func @strcmp(str1, str2: const *void, num1, num2: u32)->bool
{
    if num1 != num2
        return false
    if !str1 || !str2
        return str1 == str2
    return !@memcmp(str1, str2, num1)
}

////////////////////////////////////////////////////////////
func @abs(value: f32)->f32
{
    return value < 0 ? -value : value
}

////////////////////////////////////////////////////////////
func @print(message: string)
{
    @print(@dataof(message), @countof(message))
}

////////////////////////////////////////////////////////////
#if #os == "windows"
{
    #[swag.foreign("kernel32")]
    private func GetStdHandle(nStdHandle: u32)->*void;
    #[swag.foreign("kernel32")]
    private func WriteFile(hFile: *void, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *void)->u32;
    private const STD_OUTPUT_HANDLE: u32 = cast(u32) -11

    func @print(message: const *void, len: u32)
    {
        if !message || !len
            return
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), message, len, null, null);
    }
}