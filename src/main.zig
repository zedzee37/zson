const std = @import("std");
const expect = std.testing.expect;
const isDigit = std.ascii.isDigit;

const JsonType = enum { object, array, string, number, boolean };

const JsonElement = union(JsonType) {
    object: std.StringArrayHashMap(JsonElement),
    array: std.ArrayList(JsonElement),
    string: []u8,
    number: f32,
    boolean: bool,

    const Self = @This();

    pub fn init(comptime t: JsonType, value: anytype) Self {
        return @unionInit(Self, @tagName(t), value);
    }

    pub fn deinit(self: *Self) void {
        switch (self.*) {
            .array => {
                for (self.array.items) |*element| {
                    element.deinit();
                }
                self.array.deinit();
            },
            .object => {
                for (self.object.values()) |*value| {
                    value.deinit();
                }
                self.object.deinit();
            },
            else => {},
        }
    }
};

const DeserializerError = error{
    CouldNotParseString,
    CouldNotParseArray,
    CouldNotParseObject,
    CouldNotParseNumber,
    CouldNotParseBool,
};

const Deserializer = struct {
    file: []u8,
    current: usize,
    allocator: std.mem.Allocator,

    const Self = @This();

    pub fn deserialize(file: []u8, allocator: std.mem.Allocator) !JsonElement {
        const trimmed = try Deserializer.trimFile(file);

        if (trimmed[0] != '{' and trimmed[1] != '[') {
            std.debug.panic("Expected array or object literal, got {c}.", .{trimmed[0]});
        }

        var deserializer: Self = .{
            .file = trimmed,
            .current = 0,
            .allocator = allocator,
        };
        return deserializer.deserializeObject();
    }

    fn trimFile(file: []u8) ![]u8 {
        const allocator = std.heap.page_allocator;
        var trimmed = std.ArrayList(u8).init(allocator);

        for (file) |c| {
            if (c != '\n' and c != ' ') {
                try trimmed.append(c);
            }
        }

        return try trimmed.toOwnedSlice();
    }

    fn deserializeObject(self: *Self) DeserializerError!JsonElement {
        if (!self.match('{'))
            return self.deserializeArray();

        self.current += 1;

        var map = std.StringArrayHashMap(JsonElement).init(self.allocator);
        while (!self.match('}')) {
            const string = self.parseString() catch {
                return DeserializerError.CouldNotParseString;
            };
            self.current += string.len + 2;
            self.expect(':');

            self.current += 1;

            const obj = self.deserializeObject() catch {
                return DeserializerError.CouldNotParseObject;
            };

            map.put(string, obj) catch {
                return DeserializerError.CouldNotParseObject;
            };

            if (!self.match(',')) {
                self.expect('}');
                break;
            }

            self.current += 1;
        }
        self.current += 1;
        return JsonElement.init(JsonType.object, map);
    }

    fn deserializeArray(self: *Self) !JsonElement {
        if (!self.match('[')) {
            return self.deserializeString();
        }

        self.current += 1;
        var arr = std.ArrayList(JsonElement).init(self.allocator);
        while (!self.match(']')) {
            const obj = try self.deserializeObject();
            arr.append(obj) catch {
                return DeserializerError.CouldNotParseArray;
            };

            if (!self.match(',')) {
                self.expect(']');
                break;
            }

            self.current += 1;
        }
        self.current += 1;
        return JsonElement.init(JsonType.array, arr);
    }

    fn deserializeString(self: *Self) !JsonElement {
        const s = self.parseString() catch {
            return self.deserializeNumber();
        };
        self.current += s.len + 2;
        return JsonElement.init(JsonType.string, s);
    }

    fn deserializeNumber(self: *Self) !JsonElement {
        if (!isDigit(self.getCurrent())) {
            return self.deserializeBoolean();
        }

        var str = std.ArrayList(u8).init(self.allocator);
        var dec = false;

        while (true) {
            const current = self.getCurrent();

            if (!isDigit(current)) {
                if (!dec and current == '.') {
                    dec = true;
                    str.append(current) catch {
                        return DeserializerError.CouldNotParseNumber;
                    };
                    self.current += 1;
                    continue;
                }
                break;
            }

            str.append(current) catch {
                return DeserializerError.CouldNotParseNumber;
            };
            self.current += 1;
        }

        const slice = str.toOwnedSlice() catch {
            return DeserializerError.CouldNotParseNumber;
        };
        const parsed = std.fmt.parseFloat(f32, slice) catch {
            return DeserializerError.CouldNotParseNumber;
        };

        return JsonElement.init(JsonType.number, parsed);
    }

    fn deserializeBoolean(self: *Self) !JsonElement {
        const current = self.getCurrent();
        if (current == 't' and
            std.mem.eql(u8, self.file[self.current .. self.current + 4], "true"))
        {
            self.current += 4;
            return JsonElement.init(JsonType.boolean, true);
        } else if (current == 'f' and
            std.mem.eql(u8, self.file[self.current .. self.current + 5], "false"))
        {
            self.current += 5;
            return JsonElement.init(JsonType.boolean, false);
        }
        return DeserializerError.CouldNotParseBool;
    }

    fn parseString(self: Self) ![]u8 {
        if (!self.match('"')) {
            return DeserializerError.CouldNotParseString;
        }

        var n = self.current;
        var str = std.ArrayList(u8).init(self.allocator);
        n += 1;

        while (self.file[n] != '"') {
            str.append(self.file[n]) catch {
                return DeserializerError.CouldNotParseString;
            };
            n += 1;
        }

        return str.toOwnedSlice() catch {
            return DeserializerError.CouldNotParseString;
        };
    }

    inline fn match(self: Self, to_match: u8) bool {
        if (self.current >= self.file.len) {
            return false;
        }
        return self.getCurrent() == to_match;
    }

    inline fn getCurrent(self: Self) u8 {
        return self.file[self.current];
    }

    inline fn expect(self: Self, expected: u8) void {
        if (!self.match(expected)) {
            std.debug.panic("Expected '{c}' at {d}, got {c}.", .{ expected, self.current, self.getCurrent() });
        }
    }
};

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    const allocator = arena.allocator();
    defer arena.deinit();

    const file_arg = getFileArg(allocator);
    const file = try readFile(file_arg.?, allocator);

    _ = try Deserializer.deserialize(file, allocator);
}

pub fn getFileArg(allocator: std.mem.Allocator) ?[]u8 {
    var iterator = std.process.argsWithAllocator(allocator) catch |err| {
        std.debug.panic("{err}", .{err});
        return null;
    };
    defer iterator.deinit();

    _ = iterator.next();
    const file_name = iterator.next();

    if (file_name) |n| {
        const buf = allocator.alloc(u8, n.len) catch return null;
        std.mem.copyForwards(u8, buf, n);
        return buf;
    } else {
        std.debug.panic("Expected file as input", .{});
        return null;
    }
}

pub fn readFile(file_path: []u8, allocator: std.mem.Allocator) ![]u8 {
    const fp = try std.fs.cwd().openFile(file_path, .{});
    defer fp.close();

    const size = try fp.getEndPos();
    const buf = try allocator.alloc(u8, size);

    _ = try fp.read(buf);
    return buf;
}

test "open file" {
    const allocator = std.testing.allocator;

    var fp = "test.json".*;
    const file = try readFile(&fp, allocator);
    allocator.free(file);
}

test "Json Type" {
    var str = "guh".*;
    const val = JsonElement.init(JsonType.string, &str);

    try expect(std.mem.eql(u8, "guh", val.string));
}

test "Deserializer" {
    const allocator = std.testing.allocator;
    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();

    var fp = "test.json".*;
    const file = try readFile(&fp, arena.allocator());

    var jsonElement = try Deserializer.deserialize(file, allocator);
    defer jsonElement.deinit();

    try expect(jsonElement.object.get("guh").?.number == 103213.542141);
}
