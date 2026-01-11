#pragma once
#include <cstdint>

// constexprなchar[]からconstexprなハッシュ値を求める

namespace fnv_1a {
	static constexpr uint32_t BASE_32BIT = 0x811C9DC5;
	static constexpr uint32_t PRIME_32BIT = 0x01000193;
	static constexpr uint64_t BASE_64BIT = 0xCBF29CE484222325;
	static constexpr uint64_t PRIME_64BIT = 0x00000100000001B3;
}

// ハッシュ計算本体
constexpr uint32_t hash_u32(const char* str) {
	uint32_t h = fnv_1a::BASE_32BIT;
	for(int i = 0; str[i] != '\0'; ++i) {
		h ^= static_cast<uint32_t>(str[i]);
		h *= fnv_1a::PRIME_32BIT;
	}
	return h;
}

constexpr uint64_t hash_u64(const char* str) {
	uint64_t h = fnv_1a::BASE_64BIT;
	for(int i = 0; str[i] != '\0'; ++i) {
		h ^= static_cast<uint64_t>(str[i]);
		h *= fnv_1a::PRIME_64BIT;
	}
	return h;
}
