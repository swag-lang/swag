namespace std.text
{
    public struct EncodingUni16
    {
    }

    impl EncodingUni16
    {
        public
        {
            /// Convert an Utf8 buffer to a 16 bits unicode sequence of bytes
            func fromUtf8(dest: [..] u8, src: const [..] u8)->u32
            {
                count := 0'u32
                result := EncodingUtf8.getChar(src)
                destPtr := cast(*u16) @dataof(dest)
                while result.item1 != 0
                {
                    destPtr[0] = cast(u16) result.item1
                    destPtr += 1
                    count += 2
                    result = EncodingUtf8.getChar(result.item0)
                }

                return count
            }
        }
    }
}
