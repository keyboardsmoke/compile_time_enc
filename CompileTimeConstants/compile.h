#pragma once

#include <utility>
#include <string>

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

namespace v2
{
	namespace obfuscate
	{
		template<std::size_t N>
		class EncryptedDataContainer
		{
		public:
			EncryptedDataContainer() = delete;

			template<typename... T>
			EncryptedDataContainer(T... data) : m_data{ data... } {}

			~EncryptedDataContainer()
			{
				// Fill memory....

				memset(m_data, 0, N);
			}

			char* data()
			{
				return m_data;
			}

			std::size_t size() const
			{
				return N;
			}

			char operator[](std::size_t index) const
			{
				return m_data[index];
			}

			char& operator[](std::size_t index)
			{
				return m_data[index];
			}

		private:
			char m_data[N];
		};

		template<std::size_t N>
		class EncryptedString
		{
		public:
			EncryptedString(const EncryptedDataContainer<N>& data, const EncryptedDataContainer<N>& key) noexcept :
				m_decrypted(false), m_data(data), m_key(key) {}

			// TODO: If we inline this, sometimes the compiler just doesn't bother encrypting it on the stack
			// It leaves it decrypted and doesn't bother calling Decrypt the first pass... 
			__declspec(noinline, nothrow) char* Decrypt() noexcept
			{
				if (m_decrypted)
				{
					return m_data.data();
				}

				for (std::size_t i = 0; i < m_data.size(); ++i)
				{
					m_data[i] ^= m_key[i];
				}

				m_data[m_data.size() - 1] = 0;

				m_decrypted = true;

				return m_data.data();
			}

		private:
			bool m_decrypted : 1;
			EncryptedDataContainer<N> m_data, m_key;
		};

		namespace impl
		{
			// This seed should remain constant between compiles
			// Should be good enough to derive a key or two from...
			constexpr const uint8_t Seed = (__TIME__[0] ^ __TIME__[1] ^ __TIME__[3] ^ __TIME__[4] ^ __TIME__[6] ^ __TIME__[7]);

			template<std::size_t VArrayItemsCount, std::size_t... Is> constexpr auto XorImpl(
				std::index_sequence<Is...>, char const (&sz_text)[VArrayItemsCount], const EncryptedDataContainer<VArrayItemsCount>& sz_key) noexcept -> 
				EncryptedDataContainer<VArrayItemsCount>
			{
				return (EncryptedDataContainer<VArrayItemsCount>{static_cast<char>(sz_text[Is] ^ sz_key[Is])..., '\0'});
			}

			template<std::size_t VArrayItemsCount, std::size_t... Is> constexpr auto KeyImpl(
				std::index_sequence<Is...>, char const (&sz_text)[VArrayItemsCount]) noexcept -> 
				EncryptedDataContainer<VArrayItemsCount>
			{
				return (EncryptedDataContainer<VArrayItemsCount>{static_cast<char>(sz_text[Is] ^ impl::Seed)..., '\0'});
			}
		}

		template<std::size_t VArrayItemsCount> constexpr auto String(char const (&sz_text)[VArrayItemsCount]) -> 
			EncryptedString<VArrayItemsCount>
		{
			using sp = std::make_index_sequence<VArrayItemsCount - std::size_t{1}>;

			auto k = impl::KeyImpl<VArrayItemsCount>(sp(), sz_text);
			auto x = impl::XorImpl<VArrayItemsCount>(sp(), sz_text, k);

			return EncryptedString<VArrayItemsCount>(x, k);
		}
	}
}