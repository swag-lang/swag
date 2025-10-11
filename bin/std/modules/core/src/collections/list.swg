#global export
using Swag

// Represents a single node in a doubly-linked 'List'.
struct(T) ListEntry
{
    prev:      *ListEntry     // Pointer to the previous node in the list.
    next:      *ListEntry     // Pointer to the next node in the list.
    value:     T              // The value stored in the node.
}

// A generic, doubly-linked list.
// Provides efficient insertion and deletion of elements.
struct(T) List
{
    head:      *ListEntry     // Pointer to the first node in the list.
    tail:      *ListEntry     // Pointer to the last node in the list.
    count:     u64            // The total number of elements in the list.
}

impl List
{
    // Ensures all nodes are deallocated when the list goes out of scope.
    mtd opDrop()
    {
        .clear()
    }

    // Provides a way to iterate over each element in the list.
    // This is a macro that accepts a code block to execute for each element.
    // Visiting by pointer and in reverse order is supported.
    // Aliases:
    // - #alias0: The value of the element.
    // - #alias1: A pointer to the list node ('ListEntry').
    // - #alias2: The index of the element.
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        #if back do
            var n = .tail
        #else do
            var n = .head
        #scope(Up)
        while n
        {
            #scope(Down)
            #macro
            {
                #if #up ptr do
                    let #alias0 = cast(*T) &#up n.value
                #elif Reflection.isStruct(T) do
                    let #alias0 = cast(&T) &#up n.value
                #else do
                    let #alias0 = #up n.value
                let #alias1 = #up n
                let #alias2 = @index

                #inject(#up stmt, break = break to Up, continue = break to Down)
            }

            #if back do
                n = n.prev
            #else do
                n = n.next
        }
    }

    // Removes and deallocates all elements from the list.
    mtd clear()
    {
        var n = .head
        while n
        {
            let next = n.next
            Memory.delete(n)
            n = next
        }

        .head, .tail = null
        .count = 0
    }

    // Adds a new element to the front of the list by copying the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd addFront(value: T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = value
        .count += 1

        if .head
        {
            n.next = .head
            .head.prev, .head = n
        }
        else
        {
            .head, .tail = n
        }

        return n
    }

    // Adds a new element to the front of the list by moving the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd addFront(value: &&T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = #move value
        .count += 1

        if .head
        {
            n.next = .head
            .head.prev, .head = n
        }
        else
        {
            .head, .tail = n
        }

        return n
    }

    // Adds a new element to the back of the list by copying the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd addBack(value: T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = value
        .count += 1

        if .tail
        {
            n.prev = .tail
            .tail.next, .tail = n
        }
        else
        {
            .head, .tail = n
        }

        return n
    }

    // Adds a new element to the back of the list by moving the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd addBack(value: &&T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = #move value
        .count += 1

        if .tail
        {
            n.prev = .tail
            .tail.next, .tail = n
        }
        else
        {
            .head, .tail = n
        }

        return n
    }

    // Inserts a new element before a specified node by copying the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd insertBefore(node: *ListEntry, value: T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = value
        .count += 1

        n.next = node
        n.prev = node.prev
        if !n.prev do
            .head = n
        else do
            n.prev.next = n
        node.prev = n
        return n
    }

    // Inserts a new element before a specified node by moving the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd insertBefore(node: *ListEntry, value: &&T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = #move value
        .count += 1

        n.next = node
        n.prev = node.prev
        if !n.prev do
            .head = n
        else do
            n.prev.next = n
        node.prev = n
        return n
    }

    // Inserts a new element after a specified node by copying the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd insertAfter(node: *ListEntry, value: T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = value
        .count += 1

        n.prev = node
        n.next = node.next
        if !n.next do
            .tail = n
        else do
            n.next.prev = n
        node.next = n
        return n
    }

    // Inserts a new element after a specified node by moving the value.
    // Returns a pointer to the newly created list node.
    #[Discardable, Overload]
    mtd insertAfter(node: *ListEntry, value: &&T)->*ListEntry
    {
        let n = Memory.new'ListEntry()
        n.value = #move value
        .count += 1

        n.prev = node
        n.next = node.next
        if !n.next do
            .tail = n
        else do
            n.next.prev = n
        node.next = n
        return n
    }
}
