#global export
using Swag

struct(T) ListNode
{
    prev:   *ListNode
    next:   *ListNode
    value:  T
}

struct(T) List
{
    head:  *ListNode
    tail:  *ListNode
    count: u64
}

impl List
{
    mtd opDrop()
    {
        clear()
    }

    // Visit every elements of the list
    // @alias0 is the value
    // @alias1 is the node
    // @alias2 is the index
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        var n = head
        #scope Up
        while n
        {
            #scope Down
            #macro
            {
                #if #up ptr
                    var @alias0 = cast(*T) & #up n.value
                #elif Reflection.isStruct(T)
                    var @alias0 = cast(ref T) & #up n.value
                #else
                    var @alias0 = #up n.value
                var @alias1 = #up n
                var @alias2 = @index

                #mixin #up stmt { break = break Up; continue = break Down; }
            }

            n = n.next
        }
    }

    // Visit every elements of the list from tail to head
    // @alias0 is the value
    // @alias1 is the node
    // @alias2 is the index
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisitBack(stmt: code)
    {
        var n = tail
        #scope Up
        while n
        {
            #scope Down
            #macro
            {
                #if #up ptr
                    var @alias0 = cast(*T) & #up n.value
                #elif Reflection.isStruct(T)
                    var @alias0 = cast(ref T) & #up n.value
                #else
                    var @alias0 = #up n.value
                var @alias1 = #up n
                var @alias2 = @index

                #mixin #up stmt { break = break Up; continue = break Down; }
            }

            n = n.prev
        }
    }

    // Clear all elements
    mtd clear()
    {
        var n = head
        while n
        {
            var next = n.next
            Memory.delete(n)
            n = next
        }

        head, tail = null
        count = 0
    }

    // Add a new element on front
    #[Swag.Discardable]
    mtd addFront(value: T)->*ListNode
    {
        var n = Memory.new'ListNode()
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
    #[Swag.Discardable]
    mtd addFront(value: moveref T)->*ListNode
    {
        var n = Memory.new'ListNode()
        n.value =,move value
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
    #[Swag.Discardable]
    mtd addBack(value: T)->*ListNode
    {
        var n = Memory.new'ListNode()
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
    #[Swag.Discardable]
    mtd addBack(value: moveref T)->*ListNode
    {
        var n = Memory.new'ListNode()
        n.value =,move value
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
    #[Swag.Discardable]
    mtd insertBefore(node: *ListNode, value: T)->*ListNode
    {
        Debug.assert(node != null)

        var n = Memory.new'ListNode()
        n.value = value
        count += 1

        n.next = node
        n.prev = node.prev
        if !n.prev
            head = n
        else
            n.prev.next = n
        node.prev = n
        return n
    }

    #[Swag.Discardable]
    mtd insertBefore(node: *ListNode, value: moveref T)->*ListNode
    {
        Debug.assert(node != null)

        var n = Memory.new'ListNode()
        n.value =,move value
        count += 1

        n.next = node
        n.prev = node.prev
        if !n.prev
            head = n
        else
            n.prev.next = n
        node.prev = n
        return n
    }

    // Insert a node before the reference
    #[Swag.Discardable]
    mtd insertAfter(node: *ListNode, value: T)->*ListNode
    {
        Debug.assert(node != null)

        var n = Memory.new'ListNode()
        n.value = value
        count += 1

        n.prev = node
        n.next = node.next
        if !n.next
            tail = n
        else
            n.next.prev = n
        node.next = n
        return n
    }

    #[Swag.Discardable]
    mtd insertAfter(node: *ListNode, value: moveref T)->*ListNode
    {
        Debug.assert(node != null)

        var n = Memory.new'ListNode()
        n.value =,move value
        count += 1

        n.prev = node
        n.next = node.next
        if !n.next
            tail = n
        else
            n.next.prev = n
        node.next = n
        return n
    }
}