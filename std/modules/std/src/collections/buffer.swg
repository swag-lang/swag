namespace std.collections
{
    using swag, std.collections

    public struct Buffer
    {
        buffer:     Array'(*u8)
        allocator:  IAllocator

        count:      u32
        capacity:   u32
    }

    impl Buffer
    {    
        public
        {
            #[nodoc]
            func opCount(using self)->u32
            {
                return count
            }

            #[nodoc]
            func opDrop(using self)
            {
                free(self)
            }
        }

        public
        {
            #[nodoc]
            func free(using self)
            {
            }  
        }
    }
}