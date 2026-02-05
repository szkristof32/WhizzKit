#include <WhizzKit.h>

#include <print>

using namespace WhizzKit;

struct TestStruct
{
	uint32_t x;
	float y;
	char text[5];

	TestStruct(uint32_t _x, float _y)
		: x(_x), y(_y)
	{}
};

#define print_expected(x)	do { if (x) { std::println(#x ": {}", (size_t)(*x)); } else { std::println(#x ": error!"); } } while(false);

int main()
{
	ArenaAllocator arenaAllocator(80);
	auto memory1 = arenaAllocator.Allocate(10);
	print_expected(memory1);
	auto memory2 = arenaAllocator.Allocate(10, 16);
	print_expected(memory2);
	auto memory3 = arenaAllocator.Allocate<TestStruct>();
	print_expected(memory3);
	auto memory4 = arenaAllocator.Emplace<TestStruct>(69u, 3.14159265f);
	print_expected(memory4);
	std::println("memory4: x={}, y={}", (*memory4)->x, (*memory4)->y);
	auto memory5 = arenaAllocator.Allocate(50);
	print_expected(memory5);
}

