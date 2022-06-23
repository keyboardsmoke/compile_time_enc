#pragma once

#include <utility>
#include <string>
#include <functional>

#pragma optimize( "", off )

// #include <cstdint>

/*
namespace compile
{
	template<std::size_t N> struct MakeIndices { using type = typename MakeIndices<N - 1>::type::type; };
	template<> struct MakeIndices<0> { using type = std::index_sequence<>; };

	// Algorithms
	constexpr uint32_t SimpleXor = 0UL;
	constexpr uint32_t Add = 1UL;

	template<int AlgoID, char Key, typename I>
	class Algorithm;

	template<char Key, std::size_t... I>
	class Algorithm<SimpleXor, Key, std::index_sequence<I...>>
	{
		//
	};

	template<std::size_t... Indices>
	class String
	{
	public:
		constexpr const char TK = __TIME__[(sizeof...(Indices) ^ c) % 8];
		constexpr const char DK = __DATE__[(sizeof...(Indices) ^ c) % 11];

		constexpr __forceinline String(const char* str) : 
			key { GenerateKeyByte(str[Indices])... },
			buffer { GenerateEncryptedByte(str[Indices])... } {}

		inline const char* decrypt()
		{
			for (std::size_t i = 0; i < sizeof...(Indices); ++i)
			{
				buffer[i] = Decrypt(buffer[i], key[i]);
			}

			buffer[sizeof...(Indices)] = 0;

			return const_cast<const char*>(buffer);
		}

	private:
		constexpr char __forceinline GenerateKeyByte(char c) const
		{
			// This is only known by us, very very unlikely to be derived
			// But even if it was, this wouldn't help...
			return c ^ TK ^ DK;
		}

		constexpr char __forceinline GenerateEncryptedByte(char c) const
		{
			return c ^ GenerateKeyByte(c);
		}

		constexpr char Decrypt(char c, char k) const
		{
			return c ^ k;
		}

		volatile char key[sizeof...(I) + 1];
		volatile char buffer[sizeof...(I) + 1];
	};
}*/

/*
// V1
namespace v1
{
	constexpr const char key_byte{ '1' };

	template<std::size_t VArrayItemsCount, ::std::size_t... Is> constexpr auto obf_impl(std::index_sequence<Is...>, char const (&sz_text)[VArrayItemsCount]) -> std::array<char, VArrayItemsCount>
	{
		return (std::array<char, VArrayItemsCount>{static_cast<char>(sz_text[Is] ^ key_byte)..., '\0'});
	}

	template<std::size_t VArrayItemsCount> constexpr auto GenerateObfuscatedString(char const (&sz_text)[VArrayItemsCount]) -> std::array<char, VArrayItemsCount>
	{
		return (obf_impl<VArrayItemsCount>(std::make_index_sequence < VArrayItemsCount - std::size_t{ 1 } > (), sz_text));
	}

	class EncryptedString
	{
	public:
		EncryptedString(char* buffer, std::size_t size) :
			m_buffer(buffer), m_size(size) {}

		__declspec(noinline) std::string Decrypt() noexcept
		{
			std::string ret;

			ret.resize(m_size);

			for (std::size_t i = 0; i < m_size; ++i)
			{
				ret[i] = static_cast<char>(m_buffer[i] ^ key_byte);
			}

			ret[m_size - 1] = 0;

			return ret;
		}

	private:
		char* m_buffer;
		std::size_t m_size;
	};
}
*/

/*constexpr static auto time_from_string(const char* str, int offset) {
    return static_cast<std::uint32_t>(str[offset] - '0') * 10 +
        static_cast<std::uint32_t>(str[offset + 1] - '0');
}

constexpr static auto get_seed_constexpr() {
    auto t = __TIME__;
    return time_from_string(t, 0) * 60 * 60 + time_from_string(t, 3) * 60 + time_from_string(t, 6);
}*/

namespace obfuscate
{
	namespace constant
	{
		namespace time
		{
			template<typename T>
			constexpr static T FromString(int offset) 
			{
				return static_cast<T>(__TIME__[offset] - '0') * 10 + static_cast<T>(__TIME__[offset + 1] - '0');
			}

			template<typename T>
			constexpr static T Get() 
			{
				return FromString<T>(0) * 60 * 60 + FromString<T>(3) * 60 + FromString<T>(6);
			}
		}

		namespace hash
		{
			template<typename T, std::size_t N>
			struct FNV1A
			{
				constexpr static T hash(const char* data, T basis = static_cast<T>(2166136261))
				{
					return FNV1A<T, N - 1>::hash(data + 1, (basis ^ static_cast<T>(data[0])) * static_cast<T>(16777619));
				}
			};

			template<typename T>
			struct FNV1A<T, 0>
			{
				constexpr static T hash(const char* data, T basis)
				{
					return basis;
				}
			};
		}
	}

	template<std::size_t N>
	class EncryptedDataContainer
	{
	public:
		EncryptedDataContainer() = delete;

		template<typename... T>
		EncryptedDataContainer(T... data) : m_data{ static_cast<uint8_t>(data)... } {}

		~EncryptedDataContainer()
		{
			// Destroy memory so we don't leak on to the stack
			memset(m_data, 0, N);
		}

		char* data()
		{
			return reinterpret_cast<char*>(m_data);
		}

		uint8_t* udata()
		{
			return m_data;
		}

		std::size_t size() const
		{
			return N;
		}

		uint8_t operator[](std::size_t index) const
		{
			return m_data[index];
		}

		uint8_t& operator[](std::size_t index)
		{
			return m_data[index];
		}

	private:
		uint8_t m_data[N];
	};

	namespace impl
	{
		// Add other Impls here, can have sub/add, whatever.

		class XorImpl
		{
		public:
			template<std::size_t VArrayItemsCount, std::size_t... Is> constexpr static auto Generate(
				std::index_sequence<Is...>, char const (&sz_text)[VArrayItemsCount], uint32_t key) noexcept ->
				EncryptedDataContainer<VArrayItemsCount>
			{
				return (EncryptedDataContainer<VArrayItemsCount>{static_cast<char>(sz_text[Is] ^ key)..., '\0'});
			}

			__forceinline static uint8_t Decrypt(uint8_t c, std::size_t index, uint32_t key)
			{
				// Making this a volatile operation forces the compiler to avoid optimizing it

				volatile uint8_t r = c ^ key;

				return r;
			}
		};

		class SubAddImpl
		{
		public:
			template<std::size_t VArrayItemsCount, std::size_t... Is> constexpr static auto Generate(
				std::index_sequence<Is...>, char const (&sz_text)[VArrayItemsCount], uint32_t key) noexcept ->
				EncryptedDataContainer<VArrayItemsCount>
			{
				return (EncryptedDataContainer<VArrayItemsCount>{static_cast<char>(sz_text[Is] + key)..., '\0'});
			}

			__forceinline static uint8_t Decrypt(uint8_t c, std::size_t index, uint32_t key)
			{
				// Making this a volatile operation forces the compiler to avoid optimizing it

				volatile uint8_t r = c - key;

				return r;
			}
		};

		class CaesarImpl
		{
		public:
			// 0 -> D
			// 1 -> A
			// 2 -> 4
			// 3 -> 2
			// 4 -> 7
			// 5 -> 9
			// 6 -> 5
			// 7 -> 6
			// 8 -> B
			// 9 -> C
			// A -> F
			// B -> E
			// C -> 0
			// D -> 8
			// E -> 3
			// F -> 1

			static constexpr uint8_t EncodeTable[256] =
			{
				0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, // 00
				0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, // 10
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, // 20
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, // 30
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, // 40
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, // 50
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, // 60
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, // 70
				0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, // 80
				0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, // 90
				0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, // A0
				0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, // B0
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, // C0
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, // D0
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, // E0
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, // F0
			};

			static uint8_t DecodeTable[256];

			template<std::size_t VArrayItemsCount, std::size_t... Is> constexpr static auto Generate(
				std::index_sequence<Is...>, char const (&sz_text)[VArrayItemsCount], uint32_t key) noexcept ->
				EncryptedDataContainer<VArrayItemsCount>
			{
				return (EncryptedDataContainer<VArrayItemsCount>{static_cast<char>(EncodeTable[sz_text[Is]])..., '\0'});
			}

			__forceinline static uint8_t Decrypt(uint8_t c, std::size_t index, uint32_t key)
			{
				// Making this a volatile operation forces the compiler to avoid optimizing it

				volatile uint8_t r = DecodeTable[c];

				return r;
			}
		};
	}

	template<typename TImpl, std::size_t N>
	class EncryptedString
	{
	public:
		EncryptedString(const EncryptedString<TImpl, N>& other) noexcept :
			m_data(other.m_data), m_key(other.m_key) {}

		EncryptedString(const EncryptedDataContainer<N>& data, uint32_t key) noexcept :
			m_data(data), m_key(key) {}

		__forceinline ~EncryptedString()
		{
			volatile char* data = m_data.data();
			volatile std::size_t size = m_data.size();
			// Doesn't seem to be firing no matter what I do...
			for (std::size_t i = 0; i < size; ++i)
				data[i] = static_cast<char>(constant::hash::FNV1A<char, N>::hash(__TIME__));
		}

		// I would cache the result of this, but this is entirely stack driven
		// There's no global string value, so there's no possible re-use
		// This makes it not so performant, but don't have to worry about caching
		__forceinline char* Decrypt() noexcept
		{
			for (std::size_t i = 0; i < N; ++i)
			{
				m_data[i] = TImpl::Decrypt(m_data[i], i, m_key);
			}

			m_data[N - 1] = 0;

			return m_data.data();
		}

	private:
		uint32_t m_key;
		EncryptedDataContainer<N> m_data;
	};

	enum class StringCryptMode
	{
		Xor = 0,
		SubAdd = 1,
		Caesar = 2,

		Last,
		None = 0xff
	};

	template<std::size_t VArrayItemsCount, typename CipherT>
	class StringImpl
	{
	public:
		static constexpr auto Generate(char const (&sz_text)[VArrayItemsCount])
		{
			using sp = std::make_index_sequence < VArrayItemsCount - std::size_t{ 1 } > ;
			auto h = constant::hash::FNV1A<uint32_t, VArrayItemsCount>::hash(sz_text, VArrayItemsCount - std::size_t{ 1 });
			auto k = constant::time::Get<uint32_t>();
			auto x = CipherT::Generate<VArrayItemsCount>(sp(), sz_text, h ^ k);
			return EncryptedString<CipherT, VArrayItemsCount>(x, h ^ k);
		}
	};

	template<std::size_t Mode>
	class StringModeImpl;

	template<>
	class StringModeImpl<static_cast<std::size_t>(StringCryptMode::Xor)>
	{
	public:
		template<std::size_t VArrayItemsCount>
		static constexpr auto Generate(char const (&sz_text)[VArrayItemsCount])
		{
			return StringImpl<VArrayItemsCount, impl::XorImpl>::Generate(sz_text);
		}
	};

	template<>
	class StringModeImpl<static_cast<std::size_t>(StringCryptMode::SubAdd)>
	{
	public:
		template<std::size_t VArrayItemsCount>
		static constexpr auto Generate(char const (&sz_text)[VArrayItemsCount])
		{
			return StringImpl<VArrayItemsCount, impl::SubAddImpl>::Generate(sz_text);
		}
	};

	template<>
	class StringModeImpl<static_cast<std::size_t>(StringCryptMode::Caesar)>
	{
	public:
		template<std::size_t VArrayItemsCount>
		static constexpr auto Generate(char const (&sz_text)[VArrayItemsCount])
		{
			return StringImpl<VArrayItemsCount, impl::CaesarImpl>::Generate(sz_text);
		}
	};
}

#pragma optimize( "", on )