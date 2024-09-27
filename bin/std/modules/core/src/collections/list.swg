#global export
using Swag

struct(T) ListNode
{
    prev:      *ListNode
    next:      *ListNode
    value:     T
}

struct(T) List
{
    head:      *ListNode
    tail:      *ListNode
    count:     u64
}

impl List
{
    mtd opDrop()
    {
        clear()
    }

    // Visit every elements of the list
    // * `#alias0` is the value
    // * `#alias1` is the node
    // * `#alias2` is the index
    // Visiting by pointer and in reverse order is supported
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        #if back:
            var n = tail
        #else:
            var n = head
        #scope Up
        while n
        {
            #scope Down
            #macro
            {
                #if #up ptr:
                    let #alias0 = cast(*T) &#up n.value
                #elif Reflection.isStruct(T):
                    let #alias0 = cast(&T) &#up n.value
                #else:
                    let #alias0 = #up n.value
                let #alias1 = #up n
                let #alias2 = #index

                #inject #up stmt => { break = break to Up; continue = break to Down; }
            }

            #if back:
                n = n.prev
            #else:
                n = n.next
        }
    }

    // Clear all elements
    mtd clear()
    {
        var n = head
        while n
        {
            let next = n.next
            Memory.delete(n)
            n = next
        }

        head, tail = null
        count = 0
    }

    // Add a new element on front
    #[Discardable, Overload]
    mtd addFront(value: T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = value
        count += 1

        if head
        {
            n.next = head
            head.prev, head = n
        }
        else
        {
            head, tail = n
        }

        return n
    }

    // Add a new element on front
    #[Discardable, Overload]
    mtd addFront(value: &&T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = #move value
        count += 1

        if head
        {
            n.next = head
            head.prev, head = n
        }
        else
        {
            head, tail = n
        }

        return n
    }

    // Add a new element on front
    #[Discardable, Overload]
    mtd addBack(value: T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = value
        count += 1

        if tail
        {
            n.prev = tail
            tail.next, tail = n
        }
        else
        {
            head, tail = n
        }

        return n
    }

    // Add a new element on front
    #[Discardable, Overload]
    mtd addBack(value: &&T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = #move value
        count += 1

        if tail
        {
            n.prev = tail
            tail.next, tail = n
        }
        else
        {
            head, tail = n
        }

        return n
    }

    // Insert a node before the reference
    #[Discardable, Overload]
    mtd insertBefore(node: *ListNode, value: T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = value
        count += 1

        n.next = node
        n.prev = node.prev
        if !n.prev:
            head = n
        else:
            n.prev.next = n
        node.prev = n
        return n
    }

    #[Discardable, Overload]
    mtd insertBefore(node: *ListNode, value: &&T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = #move value
        count += 1

        n.next = node
        n.prev = node.prev
        if !n.prev:
            head = n
        else:
            n.prev.next = n
        node.prev = n
        return n
    }

    // Insert a node before the reference
    #[Discardable, Overload]
    mtd insertAfter(node: *ListNode, value: T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = value
        count += 1

        n.prev = node
        n.next = node.next
        if !n.next:
            tail = n
        else:
            n.next.prev = n
        node.next = n
        return n
    }

    #[Discardable, Overload]
    mtd insertAfter(node: *ListNode, value: &&T)->*ListNode
    {
        let n = Memory.new'ListNode()
        n.value = #move value
        count += 1

        n.prev = node
        n.next = node.next
        if !n.next:
            tail = n
        else:
            n.next.prev = n
        node.next = n
        return n
    }
}
