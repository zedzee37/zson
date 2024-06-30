const std = @import("std");
const expect = std.testing.expect;

const JsonTypeVariant = enum { object, array, string, number, boolean };

const JsonType = union(JsonTypeVariant) {
    object: std.StringHashMap(JsonType),
    array: std.ArrayList(JsonType),
    string: []u8,
    number: f32,
    boolean: bool,

    const Self = @This();

    pub fn init(comptime t: JsonTypeVariant, value: anytype) Self {
        return @unionInit(Self, @tagName(t), value);
    }
};

const Deserializer = struct {
    pub fn deserialize(file: []u8) JsonType {
        Deserializer.deserialize_object
    }

    fn deserialize_object(file: []u8, current: u32) JsonType {
    }

    fn deserialize_array(file: []u8, current: u32) JsonType {
    }

    fn deserialize_number(file: []u8, current: u32) JsonType {
    }

    fn deserialize_bool(file: []u8, current: u32) JsonType {
    }
};

pub fn main() !void {}

pub fn getFileArg() ?[]u8 {
    const allocator = std.heap.page_allocator;

    var iterator = std.process.argsWithAllocator(allocator) catch |err| {
        std.debug.panic("{err}", .{err});
        return null;
    };
    defer iterator.deinit();

    _ = iterator.next();
    const file_name = iterator.next();

    return if (file_name) |n|
        n
    else {
        std.debug.panic("Expected file as input", .{});
        return null;
    };
}

pub fn readFile(file_path: []u8, allocator: std.mem.Allocator) ![]u8 {
    const fp = try std.fs.cwd().openFile(file_path, .{});
    defer fp.close();

    const size = try fp.getEndPos();
    const buf = try allocator.alloc(u8, size);

    _ = try fp.read(buf);
    return buf;
}

test "Json Type" {
    var str = "guh".*;
    const val = JsonType.init(JsonTypeVariant.string, &str);

    try expect(std.mem.eql(u8, "guh", val.string));
}

test "open file" {
    const allocator = std.testing.allocator;

    var fp = "test.json".*;
    const file = try readFile(&fp, allocator);
    defer allocator.free(file);
}

test "Deserializer" {
    const allocator = std.testing.allocator;
    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();

    var fp = "test.json".*;
    const file = try readFile(&fp, allocator);

    const deserializer = Deserializer.deserialize(file);
}
