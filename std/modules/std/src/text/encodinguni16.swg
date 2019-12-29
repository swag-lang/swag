namespace std.text
{
    public struct EncodingUni16
    {
    }

    impl EncodingUni16
    {
        public
        {
            func fromUtf8(dest: [..] u16, src: const [..] u8)->u32
            {
                count := 0'u32
                result := EncodingUtf8.getChar(src)
                destPtr := @dataof(dest)
                while result.item1 != 0
                {
                    destPtr[0] = cast(u16) result.item1
                    destPtr += 1
                    count += 1
                    result = EncodingUtf8.getChar(result.item0)
                }

                return count
            }
        }
    }
}
