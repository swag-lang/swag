using Core, Core.Serialization

const FILE = """
{
  "first_name": "John",
  "last_name": "Smith",
  "is_alive": true,
  "age": 27,
  "address": {
    "street_address": "21 2nd Street",
    "city": "New York",
    "state": "NY",
    "postal_code": "10021-3100"
  },
  "phone_numbers": [
    {
      "type": "home",
      "number": "212 555-1234"
    },
    {
      "type": "office",
      "number": "646 555-4567"
    }
  ],
  "children": [
    "Catherine",
    "Thomas",
    "Trevor"
  ],
  "spouse": null
}
"""

struct Address
{
    street_address: String
    city: String
    state: String
    postal_code: String
}

struct Phone
{
    type:   String
    number: String
}

struct Toto
{
    first_name:     String
    last_name:      String
    is_alive:       bool
    age:            s32
    address:        Address
    phone_numbers:  Array'Phone
    children:       Array'String
    spouse:         String
}

#test
{
    var rea: Decoder'Read.JSon
    let v    = try rea.readAll'Toto(FILE)
    Console.print(dref v)
    defer Memory.delete(v)
}
