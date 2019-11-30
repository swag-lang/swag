#if swag.WINDOWS
{
    namespace std.os
    {
        #[swag.foreign("ucrtbase")]
        {
            func memcpy(dest, src: *void, size: u32)->*void;
            func memcmp(ptr1, ptr2: *void, size: u64)->s32;
            func memset(dest: *void, value: s32, size: u64)->*void;
        }
    }
}