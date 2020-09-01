namespace std.collections
{
    using std.diagnostics, std.text

    // Represents a growable buffer, which is divided in buckets to avoid a copy/realloc when
    // the buffer needs to increase its size. This is the main difference with Array
    public readonly struct ConcatBuffer
    {
        allocator:      const swag.IAllocator

        firstBucket:    *u8
        curBucket:      *u8

        totalBuckets:   u32
        usedBuckets:    u32
        curCount:       u32

        sizeBucket:     u32 = 1024
    }

    impl ConcatBuffer
    {
        func growBuckets(using self, size: u32)
        {
            if curBucket != null && curCount + size < sizeBucket
                return

            debug.assert(size <= sizeBucket)
            curCount = 0

            // No need to allocate a new bucket if we are not already using them all
            if usedBuckets < totalBuckets
            {
                ptr := cast(**void) curBucket
                curBucket = acast ptr[0]
                debug.assert(curBucket != null)
                usedBuckets += 1
                return;
            }

            if allocator == null
                allocator = @getcontext().allocator
            newBucket := acast memory.alloc(&allocator, sizeBucket + @sizeof(*void))
            if !firstBucket
            {
                firstBucket = newBucket
            }
            else
            {
                ptr := cast(**void) curBucket
                ptr[0] = newBucket
            }

            curBucket = newBucket
            newPtr := cast(**void) newBucket
            newPtr[0] = null

            usedBuckets += 1
            totalBuckets += 1
        }

        func release(using self)
        {
            if firstBucket == null
                return
            ptr := firstBucket
            while ptr != null
            {
                nextPtr := bucketToNext(self,  ptr)
                memory.free(&allocator, cast(*void) ptr)
                ptr = nextPtr
            }
        }

        func count(using self)->u32
        {
            if curBucket == null
                return 0
            return ((usedBuckets - 1) * sizeBucket) + curCount
        }

        public
        {
            func opCount(using self)->u32
            {
                return count(self)
            }

            func opDrop(using self)
            {
                release(self)
            }
        }

        public
        {
            // Associate an allocator with the buffer.
            // The allocator can only be changed if the buffer has no pending buckets.
            func setAllocator(using self, alloc: swag.IAllocator)
            {
                diagnostics.debug.assert(firstBucket == null, "bucket is not empty")
                allocator = alloc;
            }

            // Set the size of one given bucket. Minimal size is 4.
            // The buffer will be erased before the change, even if the new size is the same as the current one.
            func setBucketSize(using self, size: u32)
            {
                diagnostics.debug.assert(size >= 4)
                free(self)
                sizeBucket = size
            }

            // Release the content of the buffer
            func free(using self)
            {
                @drop(self)
                curCount, usedBuckets, totalBuckets = 0
                firstBucket, curBucket = null
            }

            // Clear the content of the buffer
            func clear(using self)
            {
                curCount = 0
                curBucket = firstBucket
                usedBuckets = curBucket ? 1 : 0
            }

            // Get the address of datas in a given bucket pointer
            #[swag.inline]
            func bucketToDatas(using self, bucket: *u8)->*u8
            {
                return bucket + @sizeof(*void)
            }

            // Get the address of the next bucket
            #[swag.inline]
            func bucketToNext(using self, bucket: *u8)->*u8
            {
                nextPtr := cast(**u8) bucket
                return nextPtr[0]
            }

            // Add one byte to the buffer
            func addByte(using self, byte: u8)
            {
                growBuckets(self, 1)
                ptr := bucketToDatas(self, curBucket) + curCount
                ptr[0] = byte
                curCount += 1
            }

            // Add a slice of bytes to the buffer
            func addBytes(using self, bytes: const [..] u8)
            {
                num := @countof(bytes)
                if !num
                    return

                slicePtr := @dataof(bytes)

                // Be sure we have a buffer
                if !curBucket
                    growBuckets(self, 1)

                // Divide the slice in the given amount of buckets if necessary
                remain := sizeBucket - curCount
                ptr := curBucket
                while(num > remain)
                {
                    ptr = bucketToDatas(self, curBucket) + curCount
                    memory.copy(ptr, slicePtr, remain)
                    num -= remain
                    curCount = sizeBucket
                    slicePtr += remain
                    if !num
                        return
                    growBuckets(self, 1)
                    remain = sizeBucket
                }

                // We should have enough size in the last bucket to store the
                // rest of the slice
                ptr = bucketToDatas(self, curBucket) + curCount
                memory.copy(ptr, slicePtr, num)
                curCount += num
            }

            // Convert buffer to a String
            func toString(using self) -> String
            {
                var result: String
                if firstBucket == null
                    return result

                ptr := firstBucket
                totalCpt := count(self)

                result.ensureCapacity(totalCpt + 1)
                while totalCpt
                {
                    offset := math.min(totalCpt, sizeBucket)
                    ptrData := bucketToDatas(self, ptr)
                    result.append(cast(string) @mkslice(ptrData, offset))

                    totalCpt -= offset
                    ptr = bucketToNext(self,  ptr)
                }

                return result
            }
        }
    }
}