#global testerror "Err0728"

struct(T) Queue
{
    struct Node
    {
        data: T
        next: *Node
    }

    gpa:        Swag.IAllocator
    start, end: *Node
}

impl Queue
{
    mtd init(alloc: Swag.IAllocator)
    {
        gpa = alloc
    }

    mtd enqueue(value: T) 
    {
        const node = try gpa.alloc(Node)
        node.data = value
        if end:
            end.next = node
        else:
            start = node
        end = node
    }

    mtd dequeue() -> T 
    {
        let start = start orelse null
        defer gpa.destroy(start)
        if start.next:
            start = next
        else:
            start, end = null
        return data;
    }        
}

#test
{
    var int_queue: Queue's32
    int_queue.init(@getcontext().allocator);

    try int_queue.enqueue(25);
    try int_queue.enqueue(50);
    try int_queue.enqueue(75);
    try int_queue.enqueue(100);
}

/*
pub fn Queue(comptime Child: type) type {
    return struct {
        const This = @This();
        const Node = struct {
            data: Child,
            next: ?*Node,
        };
        gpa: std.mem.Allocator,
        start: ?*Node,
        end: ?*Node,

        pub fn init(gpa: std.mem.Allocator) This {
            return This{
                .gpa = gpa,
                .start = null,
                .end = null,
            };
        }
        pub fn enqueue(this: *This, value: Child) !void {
            const node = try this.gpa.create(Node);
            node.* = .{ .data = value, .next = null };
            if (this.end) |end| end.next = node //
            else this.start = node;
            this.end = node;
        }
        pub fn dequeue(this: *This) ?Child {
            const start = this.start orelse return null;
            defer this.gpa.destroy(start);
            if (start.next) |next|
                this.start = next
            else {
                this.start = null;
                this.end = null;
            }
            return start.data;
        }
    };
}*/