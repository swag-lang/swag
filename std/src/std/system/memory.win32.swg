#if WINDOWS
{
    namespace std.os
    {
        #[swag.foreign("ucrtbase")]
        {
            func memcpy(dest, src: *void, size: u32);
            func memcmp(ptr1, ptr2: *void, size: u64)->s32;
            func memset(dest: *void, value: u32, size: u64);
        }
    }
}