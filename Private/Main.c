#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifdef _MSC_VER
#	define MSVC_WARNINGS(...) __pragma(warning(__VA_ARGS__))
#else
#	define MSVC_WARNINGS(...)
#endif

typedef uint8_t byte;

#define PP_STR_I(...) #__VA_ARGS__
#define PP_STR(...) PP_STR_I(__VA_ARGS__)

#define UNUSED(...) ((void)sizeof(__VA_ARGS__))

#define ARRAY_SIZE(...) (sizeof(__VA_ARGS__) / sizeof(*(__VA_ARGS__)))

#define ASSERT_TYPE_SIZE(type, size) static_assert(sizeof(type) == (size), "sizeof(" #type ") != " #size)

struct StringSpan
{
	const char* data;
	size_t size;
};

static struct StringSpan
StringSpan_Create(const char* string, size_t length)
{
	struct StringSpan span;
	span.data = string;
	span.size = length;
	return span;
}

static struct StringSpan
StringSpan_FromCString(const char* string)
{
	struct StringSpan span;
	span.data = string;
	span.size = strlen(string);
	return span;
}

static struct StringSpan
StringSpan_Substring(struct StringSpan span, size_t index, size_t length)
{
	span.data += index;
	span.size = length;
	return span;
}

static void
StringSpan_RemovePrefix(struct StringSpan* span, size_t length)
{
	span->data += length;
	span->size -= length;
}

static bool
StringSpan_Equals(struct StringSpan lhs, struct StringSpan rhs)
{
	return lhs.size == rhs.size && memcmp(lhs.data, rhs.data, lhs.size) == 0;
}

static bool
StringSpan_Equals_CString(struct StringSpan span, const char* string)
{
	size_t size = strlen(string);
	if (size != span.size)
		return false;
	return memcmp(span.data, string, size) == 0;
}

static size_t
StringSpan_FindChar(struct StringSpan text, char ch)
{
	const char* data = text.data;
	const char* last = data + text.size;

	for (; data != last; ++data)
		if (*data == ch)
			return data - text.data;

	return (size_t)-1;
}

static size_t
StringSpan_FindAnyChar(struct StringSpan text, struct StringSpan chars)
{
	const char* data = text.data;
	const char* last = data + text.size;

	for (; data != last; ++data)
		if (StringSpan_FindChar(chars, *data) != -1)
			return data - text.data;

	return (size_t)-1;
}

MSVC_WARNINGS(push)
MSVC_WARNINGS(disable: 4245)
static const uint8_t GDigits[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
MSVC_WARNINGS(pop)

static bool
ParseUInt32(struct StringSpan text, uint32_t radix, uint32_t max, uint32_t* out)
{
	const char* data = text.data;
	const char* last = data + text.size;

	while (data != last && *data == '0')
		++data;

	if (data == last)
		return false;

	uint32_t accumulate = 0;

	if (max == 0)
		max = UINT32_MAX;

	uint32_t radixCheck = max / radix;

	do {
		uint8_t digit = GDigits[*data];

		if (digit == -1)
			return false;

		if (accumulate > radixCheck)
			return false;
		accumulate *= radix;

		if (accumulate > max - digit)
			return false;
		accumulate += digit;
	} while (++data != last);

	*out = accumulate;
	return true;
}

#define STORAGE_SECTIONS(X) \
	X(3968, STORAGE1) \
	X(3968, STORAGE2) \
	X(3968, STORAGE3) \
	X(3968, STORAGE4) \
	X(3968, STORAGE5) \
	X(3968, STORAGE6) \
	X(3968, STORAGE7) \
	X(3968, STORAGE8) \
	X(2000, STORAGE9) \

#define SECTIONS(X) \
	X(3884, TRAINER) \
	X(3968, INVENTORY) \
	X(3968, RESERVED1) \
	X(3968, RESERVED2) \
	X(3848, RIVAL) \
	STORAGE_SECTIONS(X) \

#define X_ENTRY(...) +1
enum { SECTION_COUNT = 0 SECTIONS(X_ENTRY) };
#undef X_ENTRY

#define X_ENTRY(size, name, ...) SECTION_##name,
enum { SECTIONS(X_ENTRY) };
#undef X_ENTRY

#define X_ENTRY(size, name, ...) SECTION_##name##_SIZE = size,
enum { SECTIONS(X_ENTRY) };
#undef X_ENTRY

#define SECTION_INDEX(name) (SECTION_##name)
#define SECTION_SIZE(name) (SECTION_##name##_SIZE)

struct SectionInfo
{
	size_t size;
};

static const struct SectionInfo GSectionInfo[SECTION_COUNT] = {
#define X_ENTRY(size, ...) { size },
	SECTIONS(X_ENTRY)
#undef X_ENTRY
};

struct Section
{
	byte data[4080];
	byte reserved1[4];
	uint16_t index;
	uint16_t checksum;
	byte reserved2[4];
	uint32_t saveIndex;
};

static uint16_t
Section_CalculateChecksum(const struct Section* section)
{
	uint16_t index = section->index;

	const byte* data = section->data;
	size_t size = GSectionInfo[index].size;

	const uint32_t* first = (const uint32_t*)data;
	const uint32_t* last = (const uint32_t*)(data + size);

	uint32_t checksum = 0;
	while (first != last)
		checksum += *first++;

	return (checksum & 0xFFFF) + (checksum >> 16);
}

struct Save
{
	struct Section sections[SECTION_COUNT];
};

static bool
Save_GetIndex(const struct Save* save, uint32_t* out)
{
	uint32_t index = save->sections[0].saveIndex;
	for (size_t i = 1; i < SECTION_COUNT; ++i)
		if (save->sections[i].saveIndex != index)
			return false;
	*out = index;
	return true;
}

static bool
Save_GetSections(struct Save* save, struct Section** out)
{
	struct Section* sections[SECTION_COUNT];
	memset(sections, 0, sizeof(sections));

	for (size_t i = 0; i < SECTION_COUNT; ++i)
	{
		struct Section* section = &save->sections[i];

		size_t index = section->index;
		if (index >= SECTION_COUNT)
			return false;

		if (Section_CalculateChecksum(section) != section->checksum)
			return false;

		if (sections[index] != NULL)
			return false;

		sections[index] = section;
	}

	memcpy(out, sections, sizeof(sections));
	return true;
}

struct Battery
{
	struct Save saves[2];
	byte reserved[16384];
};

ASSERT_TYPE_SIZE(struct Battery, 128 * 1024);

static bool
Battery_Save(const struct Battery* battery, const char* file)
{
	FILE* stream = fopen(file, "wb");

	if (stream == NULL)
		return false;

	size_t count = fwrite(battery, sizeof(struct Battery), 1, stream);

	if (fclose(stream))
		return false;

	return count == 1;
}

static bool
Battery_Load(struct Battery* battery, const char* file)
{
	FILE* stream = fopen(file, "rb");

	if (stream == NULL)
		return false;

	size_t count = fread(battery, sizeof(struct Battery), 1, stream);

	if (getc(stream) != EOF)
		count = 0;

	if (fclose(stream))
		return false;

	return count == 1;
}

static bool
Battery_GetCurrentSave(struct Battery* battery, struct Save** save)
{
	uint32_t index1;
	struct Save* save1 = &battery->saves[0];
	if (!Save_GetIndex(save1, &index1))
		return false;

	uint32_t index2;
	struct Save* save2 = &battery->saves[1];
	if (!Save_GetIndex(save2, &index2))
		return false;

	*save = index1 > index2 ? save1 : save2;
	return true;
}

#define POKEMON_OT_NAME_SIZE 7
#define POKEMON_NICKNAME_SIZE 10

struct Pokemon_Data
{
	byte reserved[12];
};

struct Pokemon_Growth
{
	uint16_t species;
	uint16_t item;
	uint32_t experience;
	struct {
		uint8_t move4 : 2;
		uint8_t move3 : 2;
		uint8_t move2 : 2;
		uint8_t move1 : 2;
	} ppbonus;
	uint8_t friendship;
	byte reserved[2];
};
ASSERT_TYPE_SIZE(struct Pokemon_Growth, 12);

struct Pokemon_Moves
{
	uint16_t moves[4];
	uint8_t pp[4];
};
ASSERT_TYPE_SIZE(struct Pokemon_Moves, 12);

struct Pokemon_Effort
{
	uint8_t hp;
	uint8_t atk;
	uint8_t def;
	uint8_t spd;
	uint8_t spatk;
	uint8_t spdef;

	uint8_t coolness;
	uint8_t beauty;
	uint8_t cuteness;
	uint8_t smartness;
	uint8_t toughness;
	uint8_t feel;
};
ASSERT_TYPE_SIZE(struct Pokemon_Effort, 12);

struct Pokemon_Misc
{
	uint8_t pokerus;
	uint8_t location;
	uint16_t origin;
	uint32_t values;
	uint32_t ribbon;
};
ASSERT_TYPE_SIZE(struct Pokemon_Misc, 12);

struct Pokemon_Misc_Unpacked
{
	uint8_t pokerus;
	uint8_t location;

	struct {
		uint8_t gender;
		uint8_t ball;
		uint8_t game;
		uint8_t level;
	} origin;

	struct {
		uint8_t ability;
		uint8_t egg;
		uint8_t spdef;
		uint8_t spatk;
		uint8_t spd;
		uint8_t def;
		uint8_t atk;
		uint8_t hp;
	} values;

	struct {
		uint8_t obedience;
		uint8_t reserved;
		uint8_t special6;
		uint8_t special5;
		uint8_t special4;
		uint8_t special3;
		uint8_t special2;
		uint8_t special1;
		uint8_t effort;
		uint8_t artist;
		uint8_t victory;
		uint8_t winning;
		uint8_t champion;
		uint8_t tough;
		uint8_t smart;
		uint8_t cute;
		uint8_t beauty;
		uint8_t cool;
	} ribbon;
};

#define SET_BITS(x, o, c, v) ((x) | (((v) & ~(-1 << c)) << o))
#define GET_BITS(x, o, c) ((x) >> (o) & ~(-1 << c))

static void
Pokemon_Misc_Pack(struct Pokemon_Misc* misc, const struct Pokemon_Misc_Unpacked* unpacked)
{
	misc->pokerus = unpacked->pokerus;
	misc->location = unpacked->location;

	{
		uint16_t x = 0;
		x = SET_BITS(x,  0, 7, unpacked->origin.level);
		x = SET_BITS(x,  7, 4, unpacked->origin.game);
		x = SET_BITS(x, 11, 4, unpacked->origin.ball);
		x = SET_BITS(x, 15, 1, unpacked->origin.gender);
		misc->origin = x;
	}

	{
		uint32_t x = 0;
		x = SET_BITS(x,  0, 5, unpacked->values.hp);
		x = SET_BITS(x,  5, 5, unpacked->values.atk);
		x = SET_BITS(x, 10, 5, unpacked->values.def);
		x = SET_BITS(x, 15, 5, unpacked->values.spd);
		x = SET_BITS(x, 20, 5, unpacked->values.spatk);
		x = SET_BITS(x, 25, 5, unpacked->values.spdef);
		x = SET_BITS(x, 30, 1, unpacked->values.egg);
		x = SET_BITS(x, 31, 1, unpacked->values.ability);
		misc->values = x;
	}

	{
		uint32_t x = 0;
		x = SET_BITS(x,  0, 3, unpacked->ribbon.cool);
		x = SET_BITS(x,  3, 3, unpacked->ribbon.beauty);
		x = SET_BITS(x,  6, 3, unpacked->ribbon.cute);
		x = SET_BITS(x,  9, 3, unpacked->ribbon.smart);
		x = SET_BITS(x, 12, 3, unpacked->ribbon.tough);
		x = SET_BITS(x, 15, 1, unpacked->ribbon.champion);
		x = SET_BITS(x, 16, 1, unpacked->ribbon.winning);
		x = SET_BITS(x, 17, 1, unpacked->ribbon.victory);
		x = SET_BITS(x, 18, 1, unpacked->ribbon.artist);
		x = SET_BITS(x, 19, 1, unpacked->ribbon.effort);
		x = SET_BITS(x, 20, 1, unpacked->ribbon.special1);
		x = SET_BITS(x, 21, 1, unpacked->ribbon.special2);
		x = SET_BITS(x, 22, 1, unpacked->ribbon.special3);
		x = SET_BITS(x, 23, 1, unpacked->ribbon.special4);
		x = SET_BITS(x, 24, 1, unpacked->ribbon.special5);
		x = SET_BITS(x, 25, 1, unpacked->ribbon.special6);
		x = SET_BITS(x, 31, 1, unpacked->ribbon.obedience);
		misc->ribbon = x;
	}
}

static void
Pokemon_Misc_Unpack(const struct Pokemon_Misc* misc, struct Pokemon_Misc_Unpacked* unpacked)
{
	unpacked->pokerus = misc->pokerus;
	unpacked->location = misc->location;

	{
		uint16_t x = misc->origin;
		unpacked->origin.level  = GET_BITS(x,  0, 7);
		unpacked->origin.game   = GET_BITS(x,  7, 4);
		unpacked->origin.ball   = GET_BITS(x, 11, 4);
		unpacked->origin.gender = GET_BITS(x, 15, 1);
	}

	{
		uint32_t x = misc->values;
		unpacked->values.hp      = GET_BITS(x,  0, 5);
		unpacked->values.atk     = GET_BITS(x,  5, 5);
		unpacked->values.def     = GET_BITS(x, 10, 5);
		unpacked->values.spd     = GET_BITS(x, 15, 5);
		unpacked->values.spatk   = GET_BITS(x, 20, 5);
		unpacked->values.spdef   = GET_BITS(x, 25, 5);
		unpacked->values.egg     = GET_BITS(x, 30, 1);
		unpacked->values.ability = GET_BITS(x, 31, 1);
	}

	{
		uint32_t x = misc->ribbon;
		unpacked->ribbon.cool      = GET_BITS(x,  0, 3);
		unpacked->ribbon.beauty    = GET_BITS(x,  3, 3);
		unpacked->ribbon.cute      = GET_BITS(x,  6, 3);
		unpacked->ribbon.smart     = GET_BITS(x,  9, 3);
		unpacked->ribbon.tough     = GET_BITS(x, 12, 3);
		unpacked->ribbon.champion  = GET_BITS(x, 15, 1);
		unpacked->ribbon.winning   = GET_BITS(x, 16, 1);
		unpacked->ribbon.victory   = GET_BITS(x, 17, 1);
		unpacked->ribbon.artist    = GET_BITS(x, 18, 1);
		unpacked->ribbon.effort    = GET_BITS(x, 19, 1);
		unpacked->ribbon.special1  = GET_BITS(x, 20, 1);
		unpacked->ribbon.special2  = GET_BITS(x, 21, 1);
		unpacked->ribbon.special3  = GET_BITS(x, 22, 1);
		unpacked->ribbon.special4  = GET_BITS(x, 23, 1);
		unpacked->ribbon.special5  = GET_BITS(x, 24, 1);
		unpacked->ribbon.special6  = GET_BITS(x, 25, 1);
		unpacked->ribbon.obedience = GET_BITS(x, 31, 1);
	}
}

struct Pokemon
{
	uint32_t personality;
	union {
		uint32_t trainer;
		struct {
			uint16_t trainerPublic;
			uint16_t trainerSecret;
		};
	};
	byte nickname[POKEMON_NICKNAME_SIZE];
	uint16_t language;
	byte trainerName[POKEMON_OT_NAME_SIZE];
	uint8_t markings;
	uint16_t checksum;
	byte reserved[2];

	union {
		struct Pokemon_Data data[4];
		struct {
			struct Pokemon_Growth growth;
			struct Pokemon_Moves moves;
			struct Pokemon_Effort effort;
			struct Pokemon_Misc misc;
		};
	} data;
};

ASSERT_TYPE_SIZE(struct Pokemon, 80);

static bool
Pokemon_Exists(const struct Pokemon* pokemon)
{
	static const byte null[POKEMON_NICKNAME_SIZE] = { 0 };
	return memcmp(pokemon->nickname, null, sizeof(null)) != 0;
}

static void
Pokemon_Encrypt(struct Pokemon* pokemon)
{
	uint32_t key = pokemon->trainer ^ pokemon->personality;

	byte* data = (byte*)&pokemon->data;

	uint32_t* first = (uint32_t*)data;
	uint32_t* last = (uint32_t*)(data + sizeof(pokemon->data));

	while (first != last)
		*first++ ^= key;
}

static void
Pokemon_Decrypt(struct Pokemon* pokemon)
{
	Pokemon_Encrypt(pokemon);
}

static uint16_t
Pokemon_CalculateChecksum(const struct Pokemon* pokemon)
{
	const uint16_t* data = (const uint16_t*)&pokemon->data;
	size_t size = sizeof(pokemon->data) / sizeof(uint16_t);

	uint16_t checksum = 0;
	for (size_t i = 0; i < size; ++i)
		checksum += data[i];

	return checksum;
}

static const uint8_t GScramble[24][4] = {
	{ 0, 1, 2, 3 },
	{ 0, 1, 3, 2 },
	{ 0, 2, 1, 3 },
	{ 0, 2, 3, 1 },
	{ 0, 3, 1, 2 },
	{ 0, 3, 2, 1 },
	{ 1, 0, 2, 3 },
	{ 1, 0, 3, 2 },
	{ 1, 2, 0, 3 },
	{ 1, 2, 3, 0 },
	{ 1, 3, 0, 2 },
	{ 1, 3, 2, 0 },
	{ 2, 0, 1, 3 },
	{ 2, 0, 3, 1 },
	{ 2, 1, 0, 3 },
	{ 2, 1, 3, 0 },
	{ 2, 3, 0, 1 },
	{ 2, 3, 1, 0 },
	{ 3, 0, 1, 2 },
	{ 3, 0, 2, 1 },
	{ 3, 1, 0, 2 },
	{ 3, 1, 2, 0 },
	{ 3, 2, 0, 1 },
	{ 3, 2, 1, 0 },
};

static void
Pokemon_Scramble(struct Pokemon* pokemon)
{
	const uint8_t* scramble = GScramble[pokemon->personality % 24];

	struct Pokemon_Data data[4];
	memcpy(data, pokemon->data.data, sizeof(data));

	pokemon->data.data[0] = data[scramble[0]];
	pokemon->data.data[1] = data[scramble[1]];
	pokemon->data.data[2] = data[scramble[2]];
	pokemon->data.data[3] = data[scramble[3]];
}

static void
Pokemon_Unscramble(struct Pokemon* pokemon)
{
	const uint8_t* scramble = GScramble[pokemon->personality % 24];

	struct Pokemon_Data data[4];
	memcpy(data, pokemon->data.data, sizeof(data));

	pokemon->data.data[scramble[0]] = data[0];
	pokemon->data.data[scramble[1]] = data[1];
	pokemon->data.data[scramble[2]] = data[2];
	pokemon->data.data[scramble[3]] = data[3];
}

struct PokemonInfo
{
	uint16_t index;
	char name[POKEMON_NICKNAME_SIZE + 1];
};

static const struct PokemonInfo GPokemon[] = {
	{   0, "??????????" },
	{   1, "Bulbasaur"  },
	{   2, "Ivysaur"    },
	{   3, "Venusaur"   },
	{   4, "Charmander" },
	{   5, "Charmeleon" },
	{   6, "Charizard"  },
	{   7, "Squirtle"   },
	{   8, "Wartortle"  },
	{   9, "Blastoise"  },
	{  10, "Caterpie"   },
	{  11, "Metapod"    },
	{  12, "Butterfree" },
	{  13, "Weedle"     },
	{  14, "Kakuna"     },
	{  15, "Beedrill"   },
	{  16, "Pidgey"     },
	{  17, "Pidgeotto"  },
	{  18, "Pidgeot"    },
	{  19, "Rattata"    },
	{  20, "Raticate"   },
	{  21, "Spearow"    },
	{  22, "Fearow"     },
	{  23, "Ekans"      },
	{  24, "Arbok"      },
	{  25, "Pikachu"    },
	{  26, "Raichu"     },
	{  27, "Sandshrew"  },
	{  28, "Sandslash"  },
	{  29, "NidoranF"   },
	{  30, "Nidorina"   },
	{  31, "Nidoqueen"  },
	{  32, "NidoranM"   },
	{  33, "Nidorino"   },
	{  34, "Nidoking"   },
	{  35, "Clefairy"   },
	{  36, "Clefable"   },
	{  37, "Vulpix"     },
	{  38, "Ninetales"  },
	{  39, "Jigglypuff" },
	{  40, "Wigglytuff" },
	{  41, "Zubat"      },
	{  42, "Golbat"     },
	{  43, "Oddish"     },
	{  44, "Gloom"      },
	{  45, "Vileplume"  },
	{  46, "Paras"      },
	{  47, "Parasect"   },
	{  48, "Venonat"    },
	{  49, "Venomoth"   },
	{  50, "Diglett"    },
	{  51, "Dugtrio"    },
	{  52, "Meowth"     },
	{  53, "Persian"    },
	{  54, "Psyduck"    },
	{  55, "Golduck"    },
	{  56, "Mankey"     },
	{  57, "Primeape"   },
	{  58, "Growlithe"  },
	{  59, "Arcanine"   },
	{  60, "Poliwag"    },
	{  61, "Poliwhirl"  },
	{  62, "Poliwrath"  },
	{  63, "Abra"       },
	{  64, "Kadabra"    },
	{  65, "Alakazam"   },
	{  66, "Machop"     },
	{  67, "Machoke"    },
	{  68, "Machamp"    },
	{  69, "Bellsprout" },
	{  70, "Weepinbell" },
	{  71, "Victreebel" },
	{  72, "Tentacool"  },
	{  73, "Tentacruel" },
	{  74, "Geodude"    },
	{  75, "Graveler"   },
	{  76, "Golem"      },
	{  77, "Ponyta"     },
	{  78, "Rapidash"   },
	{  79, "Slowpoke"   },
	{  80, "Slowbro"    },
	{  81, "Magnemite"  },
	{  82, "Magneton"   },
	{  83, "Farfetch'd" },
	{  84, "Doduo"      },
	{  85, "Dodrio"     },
	{  86, "Seel"       },
	{  87, "Dewgong"    },
	{  88, "Grimer"     },
	{  89, "Muk"        },
	{  90, "Shellder"   },
	{  91, "Cloyster"   },
	{  92, "Gastly"     },
	{  93, "Haunter"    },
	{  94, "Gengar"     },
	{  95, "Onix"       },
	{  96, "Drowzee"    },
	{  97, "Hypno"      },
	{  98, "Krabby"     },
	{  99, "Kingler"    },
	{ 100, "Voltorb"    },
	{ 101, "Electrode"  },
	{ 102, "Exeggcute"  },
	{ 103, "Exeggutor"  },
	{ 104, "Cubone"     },
	{ 105, "Marowak"    },
	{ 106, "Hitmonlee"  },
	{ 107, "Hitmonchan" },
	{ 108, "Lickitung"  },
	{ 109, "Koffing"    },
	{ 110, "Weezing"    },
	{ 111, "Rhyhorn"    },
	{ 112, "Rhydon"     },
	{ 113, "Chansey"    },
	{ 114, "Tangela"    },
	{ 115, "Kangaskhan" },
	{ 116, "Horsea"     },
	{ 117, "Seadra"     },
	{ 118, "Goldeen"    },
	{ 119, "Seaking"    },
	{ 120, "Staryu"     },
	{ 121, "Starmie"    },
	{ 122, "Mr. Mime"   },
	{ 123, "Scyther"    },
	{ 124, "Jynx"       },
	{ 125, "Electabuzz" },
	{ 126, "Magmar"     },
	{ 127, "Pinsir"     },
	{ 128, "Tauros"     },
	{ 129, "Magikarp"   },
	{ 130, "Gyarados"   },
	{ 131, "Lapras"     },
	{ 132, "Ditto"      },
	{ 133, "Eevee"      },
	{ 134, "Vaporeon"   },
	{ 135, "Jolteon"    },
	{ 136, "Flareon"    },
	{ 137, "Porygon"    },
	{ 138, "Omanyte"    },
	{ 139, "Omastar"    },
	{ 140, "Kabuto"     },
	{ 141, "Kabutops"   },
	{ 142, "Aerodactyl" },
	{ 143, "Snorlax"    },
	{ 144, "Articuno"   },
	{ 145, "Zapdos"     },
	{ 146, "Moltres"    },
	{ 147, "Dratini"    },
	{ 148, "Dragonair"  },
	{ 149, "Dragonite"  },
	{ 150, "Mewtwo"     },
	{ 151, "Mew"        },
	{ 152, "Chikorita"  },
	{ 153, "Bayleef"    },
	{ 154, "Meganium"   },
	{ 155, "Cyndaquil"  },
	{ 156, "Quilava"    },
	{ 157, "Typhlosion" },
	{ 158, "Totodile"   },
	{ 159, "Croconaw"   },
	{ 160, "Feraligatr" },
	{ 161, "Sentret"    },
	{ 162, "Furret"     },
	{ 163, "Hoothoot"   },
	{ 164, "Noctowl"    },
	{ 165, "Ledyba"     },
	{ 166, "Ledian"     },
	{ 167, "Spinarak"   },
	{ 168, "Ariados"    },
	{ 169, "Crobat"     },
	{ 170, "Chinchou"   },
	{ 171, "Lanturn"    },
	{ 172, "Pichu"      },
	{ 173, "Cleffa"     },
	{ 174, "Igglybuff"  },
	{ 175, "Togepi"     },
	{ 176, "Togetic"    },
	{ 177, "Natu"       },
	{ 178, "Xatu"       },
	{ 179, "Mareep"     },
	{ 180, "Flaaffy"    },
	{ 181, "Ampharos"   },
	{ 182, "Bellossom"  },
	{ 183, "Marill"     },
	{ 184, "Azumarill"  },
	{ 185, "Sudowoodo"  },
	{ 186, "Politoed"   },
	{ 187, "Hoppip"     },
	{ 188, "Skiploom"   },
	{ 189, "Jumpluff"   },
	{ 190, "Aipom"      },
	{ 191, "Sunkern"    },
	{ 192, "Sunflora"   },
	{ 193, "Yanma"      },
	{ 194, "Wooper"     },
	{ 195, "Quagsire"   },
	{ 196, "Espeon"     },
	{ 197, "Umbreon"    },
	{ 198, "Murkrow"    },
	{ 199, "Slowking"   },
	{ 200, "Misdreavus" },
	{ 201, "Unown"      },
	{ 202, "Wobbuffet"  },
	{ 203, "Girafarig"  },
	{ 204, "Pineco"     },
	{ 205, "Forretress" },
	{ 206, "Dunsparce"  },
	{ 207, "Gligar"     },
	{ 208, "Steelix"    },
	{ 209, "Snubbull"   },
	{ 210, "Granbull"   },
	{ 211, "Qwilfish"   },
	{ 212, "Scizor"     },
	{ 213, "Shuckle"    },
	{ 214, "Heracross"  },
	{ 215, "Sneasel"    },
	{ 216, "Teddiursa"  },
	{ 217, "Ursaring"   },
	{ 218, "Slugma"     },
	{ 219, "Magcargo"   },
	{ 220, "Swinub"     },
	{ 221, "Piloswine"  },
	{ 222, "Corsola"    },
	{ 223, "Remoraid"   },
	{ 224, "Octillery"  },
	{ 225, "Delibird"   },
	{ 226, "Mantine"    },
	{ 227, "Skarmory"   },
	{ 228, "Houndour"   },
	{ 229, "Houndoom"   },
	{ 230, "Kingdra"    },
	{ 231, "Phanpy"     },
	{ 232, "Donphan"    },
	{ 233, "Porygon2"   },
	{ 234, "Stantler"   },
	{ 235, "Smeargle"   },
	{ 236, "Tyrogue"    },
	{ 237, "Hitmontop"  },
	{ 238, "Smoochum"   },
	{ 239, "Elekid"     },
	{ 240, "Magby"      },
	{ 241, "Miltank"    },
	{ 242, "Blissey"    },
	{ 243, "Raikou"     },
	{ 244, "Entei"      },
	{ 245, "Suicune"    },
	{ 246, "Larvitar"   },
	{ 247, "Pupitar"    },
	{ 248, "Tyranitar"  },
	{ 249, "Lugia"      },
	{ 250, "Ho-Oh"      },
	{ 251, "Celebi"     },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{   0, "??????????" },
	{ 252, "Treecko"    },
	{ 253, "Grovyle"    },
	{ 254, "Sceptile"   },
	{ 255, "Torchic"    },
	{ 256, "Combusken"  },
	{ 257, "Blaziken"   },
	{ 258, "Mudkip"     },
	{ 259, "Marshtomp"  },
	{ 260, "Swampert"   },
	{ 261, "Poochyena"  },
	{ 262, "Mightyena"  },
	{ 263, "Zigzagoon"  },
	{ 264, "Linoone"    },
	{ 265, "Wurmple"    },
	{ 266, "Silcoon"    },
	{ 267, "Beautifly"  },
	{ 268, "Cascoon"    },
	{ 269, "Dustox"     },
	{ 270, "Lotad"      },
	{ 271, "Lombre"     },
	{ 272, "Ludicolo"   },
	{ 273, "Seedot"     },
	{ 274, "Nuzleaf"    },
	{ 275, "Shiftry"    },
	{ 290, "Nincada"    },
	{ 291, "Ninjask"    },
	{ 292, "Shedinja"   },
	{ 276, "Taillow"    },
	{ 277, "Swellow"    },
	{ 285, "Shroomish"  },
	{ 286, "Breloom"    },
	{ 327, "Spinda"     },
	{ 278, "Wingull"    },
	{ 279, "Pelipper"   },
	{ 283, "Surskit"    },
	{ 284, "Masquerain" },
	{ 320, "Wailmer"    },
	{ 321, "Wailord"    },
	{ 300, "Skitty"     },
	{ 301, "Delcatty"   },
	{ 352, "Kecleon"    },
	{ 343, "Baltoy"     },
	{ 344, "Claydol"    },
	{ 299, "Nosepass"   },
	{ 324, "Torkoal"    },
	{ 302, "Sableye"    },
	{ 339, "Barboach"   },
	{ 340, "Whiscash"   },
	{ 370, "Luvdisc"    },
	{ 341, "Corphish"   },
	{ 342, "Crawdaunt"  },
	{ 349, "Feebas"     },
	{ 350, "Milotic"    },
	{ 318, "Carvanha"   },
	{ 319, "Sharpedo"   },
	{ 328, "Trapinch"   },
	{ 329, "Vibrava"    },
	{ 330, "Flygon"     },
	{ 296, "Makuhita"   },
	{ 297, "Hariyama"   },
	{ 309, "Electrike"  },
	{ 310, "Manectric"  },
	{ 322, "Numel"      },
	{ 323, "Camerupt"   },
	{ 363, "Spheal"     },
	{ 364, "Sealeo"     },
	{ 365, "Walrein"    },
	{ 331, "Cacnea"     },
	{ 332, "Cacturne"   },
	{ 361, "Snorunt"    },
	{ 362, "Glalie"     },
	{ 337, "Lunatone"   },
	{ 338, "Solrock"    },
	{ 298, "Azurill"    },
	{ 325, "Spoink"     },
	{ 326, "Grumpig"    },
	{ 311, "Plusle"     },
	{ 312, "Minun"      },
	{ 303, "Mawile"     },
	{ 307, "Meditite"   },
	{ 308, "Medicham"   },
	{ 333, "Swablu"     },
	{ 334, "Altaria"    },
	{ 360, "Wynaut"     },
	{ 355, "Duskull"    },
	{ 356, "Dusclops"   },
	{ 315, "Roselia"    },
	{ 287, "Slakoth"    },
	{ 288, "Vigoroth"   },
	{ 289, "Slaking"    },
	{ 316, "Gulpin"     },
	{ 317, "Swalot"     },
	{ 357, "Tropius"    },
	{ 293, "Whismur"    },
	{ 294, "Loudred"    },
	{ 295, "Exploud"    },
	{ 366, "Clamperl"   },
	{ 367, "Huntail"    },
	{ 368, "Gorebyss"   },
	{ 359, "Absol"      },
	{ 353, "Shuppet"    },
	{ 354, "Banette"    },
	{ 336, "Seviper"    },
	{ 335, "Zangoose"   },
	{ 369, "Relicanth"  },
	{ 304, "Aron"       },
	{ 305, "Lairon"     },
	{ 306, "Aggron"     },
	{ 351, "Castform"   },
	{ 313, "Volbeat"    },
	{ 314, "Illumise"   },
	{ 345, "Lileep"     },
	{ 346, "Cradily"    },
	{ 347, "Anorith"    },
	{ 348, "Armaldo"    },
	{ 280, "Ralts"      },
	{ 281, "Kirlia"     },
	{ 282, "Gardevoir"  },
	{ 371, "Bagon"      },
	{ 372, "Shelgon"    },
	{ 373, "Salamence"  },
	{ 374, "Beldum"     },
	{ 375, "Metang"     },
	{ 376, "Metagross"  },
	{ 377, "Regirock"   },
	{ 378, "Regice"     },
	{ 379, "Registeel"  },
	{ 382, "Kyogre"     },
	{ 383, "Groudon"    },
	{ 384, "Rayquaza"   },
	{ 380, "Latias"     },
	{ 381, "Latios"     },
	{ 385, "Jirachi"    },
	{ 386, "Deoxys"     },
	{ 358, "Chimecho"   },
};

#define STORAGE_BOX_SIZE 30
#define STORAGE_BOX_ROWS 5
#define STORAGE_BOX_COLS 6
#define STORAGE_BOX_COUNT 14
#define STORAGE_NAME_SIZE 9

struct PokemonStorage
{
	uint32_t current;
	struct Pokemon pokemon[STORAGE_BOX_SIZE * STORAGE_BOX_COUNT];
	byte names[STORAGE_NAME_SIZE * STORAGE_BOX_COUNT];
	uint8_t wallpapers[STORAGE_BOX_COUNT];
};

static bool
PokemonStorage_Save(const struct PokemonStorage* storage, struct Section* const* sections)
{
	const byte* buffer = (const byte*)storage;

#define X_ENTRY(size, name, ...) { \
		size_t index = SECTION_INDEX(name); \
		struct Section* section = sections[index]; \
		size_t offset = (index - SECTION_STORAGE1) * SECTION_STORAGE1_SIZE; \
		memcpy(section->data, buffer + offset, SECTION_SIZE(name)); \
		section->checksum = Section_CalculateChecksum(section); \
	}
	STORAGE_SECTIONS(X_ENTRY);
#undef X_ENTRY

	return true;
}

static bool
PokemonStorage_Load(struct PokemonStorage* storage, const struct Section* const* sections)
{
	byte* buffer = (byte*)storage;

#define X_ENTRY(size, name, ...) { \
		size_t index = SECTION_INDEX(name); \
		const struct Section* section = sections[index]; \
		size_t offset = (index - SECTION_STORAGE1) * SECTION_STORAGE1_SIZE; \
		memcpy(buffer + offset, section->data, SECTION_SIZE(name)); \
	}
	STORAGE_SECTIONS(X_ENTRY);
#undef X_ENTRY

	return true;
}

static const byte GStringEncodeTable[256] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

	0x00, 0xAB, 0xB2, 0x50, 0xB7, 0x5B, 0x2D, 0xB4, 0x5C, 0x5D, 0xAF, 0x2E, 0xB8, 0xAE, 0xAD, 0xBA,
	0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xF0, 0x36, 0x85, 0x35, 0x86, 0xAC,

	0x50, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
	0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x5C, 0x50, 0x5D, 0x50, 0x50,

	0x50, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3,
	0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0x5C, 0x50, 0x5D, 0x50, 0xFF,

	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static void
String_Encode(const char* string, size_t length, byte* buffer, size_t bufferSize)
{
	if (bufferSize == 0)
		return;

	byte* bufferLast = buffer + bufferSize;
	const char* stringLast = string + length;
	for (; buffer != bufferLast && string != stringLast; ++buffer, ++string)
		if ((*buffer = GStringEncodeTable[*string]) == 0xFF)
			return;
	if (buffer != bufferLast)
		*buffer = 0xFF;
}

static const char GStringDecodeTable[256] = {
	0x20, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,

	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x26, 0x2B, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x3D, 0x3B, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,

	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x25, 0x28, 0x29, 0x1A, 0x1A,

	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,

	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x3C, 0x3E, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,

	0x1A, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x21, 0x3F, 0x2E, 0x2D, 0x2A,
	0x1A, 0x22, 0x22, 0x27, 0x27, 0x1A, 0x1A, 0x24, 0x2C, 0x1A, 0x2F, 0x41, 0x42, 0x43, 0x44, 0x45,

	0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,
	0x56, 0x57, 0x58, 0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,

	0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x1A,
	0x3A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x00,
};

static void
String_Decode(const byte* string, size_t length, char* buffer, size_t bufferSize)
{
	if (bufferSize == 0)
		return;

	if (length >= bufferSize)
		length = bufferSize - 1;

	for (const byte* last = string + length; string != last; ++string)
	{
		byte input = *string;

		if (input == 0xFF)
			break;

		*buffer++ = GStringDecodeTable[input];
	}

	*buffer = 0;
}

#define CONTEXT_SIZE (sizeof(void*) * 2)
#define CONTEXT(type) (*(sizeof(int[sizeof(type) <= CONTEXT_SIZE ? 1 : -1]), (const type*)context))
#define CONTEXT_SET(type) (*(sizeof(int[sizeof(type) <= CONTEXT_SIZE ? 1 : -1]), (type*)context))

typedef bool FnParseContext(void* context, struct StringSpan text);

static bool
ParseContext_StringSpan(void* context, struct StringSpan text)
{
	CONTEXT_SET(struct StringSpan) = text;
	return true;
}

static bool
ParseContext_uint8(void* context, struct StringSpan text)
{
	uint32_t value;
	bool result = ParseUInt32(text, 10, UINT8_MAX, &value);
	*(uint8_t*)context = (uint8_t)value;
	return result;
}

static bool
ParseContext_uint16(void* context, struct StringSpan text)
{
	uint32_t value;
	bool result = ParseUInt32(text, 10, UINT16_MAX, &value);
	*(uint16_t*)context = (uint16_t)value;
	return result;
}

static bool
ParseContext_uint32(void* context, struct StringSpan text)
{
	return ParseUInt32(text, 10, 0, (uint32_t*)context);
}

static bool
ParseContext_Gender(void* context, struct StringSpan text)
{
	if (text.size != 1)
		return false;

	bool gender;
	switch (*text.data)
	{
	case 'm':
	case 'M':
		gender = false;
		break;

	case 'f':
	case 'F':
		gender = true;
		break;

	default:
		return false;
	}

	CONTEXT_SET(bool) = gender;
	return true;
}

typedef bool FnFilter(const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index, const void* context);
typedef void FnAction(struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc, const void* context);

struct Filter
{
	FnFilter* func;
	byte context[CONTEXT_SIZE];
	bool expect;
};

static bool
Filter_Invoke(const struct Filter* filter, const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index)
{
	return filter->func(pokemon, misc, index, &filter->context) == filter->expect;
}

static bool
Filters_Box(const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index, const void* context)
{
	UNUSED(pokemon, misc);

	return (index / STORAGE_BOX_SIZE) + 1 == CONTEXT(uint32_t);
}

static bool
Filters_Slot(const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index, const void* context)
{
	UNUSED(pokemon, misc);

	return (index % STORAGE_BOX_SIZE) + 1 == CONTEXT(uint32_t);
}

static bool
Filters_Pokedex(const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index, const void* context)
{
	UNUSED(misc, index);

	return GPokemon[pokemon->data.growth.species].index == CONTEXT(uint16_t);
}

static bool
Filters_Trainer(const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index, const void* context)
{
	UNUSED(misc, index);

	return pokemon->trainerPublic == CONTEXT(uint16_t);
}

static bool
Filters_TrainerGender(const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index, const void* context)
{
	UNUSED(pokemon, index);

	return misc->origin.gender == CONTEXT(bool);
}

struct FilterInfo
{
	const char* name;
	FnFilter* func;
	FnParseContext* parseContext;
};

struct FilterInfo const GFilters[] = {
	{ "box", Filters_Box, ParseContext_uint32 },
	{ "slot", Filters_Slot, ParseContext_uint32 },
	{ "pokedex", Filters_Pokedex, ParseContext_uint16 },
	{ "trainer-id", Filters_Trainer, ParseContext_uint16 },
	{ "trainer-gender", Filters_TrainerGender, ParseContext_Gender },
};

struct Action
{
	FnAction* func;
	byte context[CONTEXT_SIZE];
};

static void
Action_Invoke(const struct Action* action, struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc)
{
	action->func(pokemon, misc, &action->context);
}

static void
Actions_Nickname(struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc, const void* context)
{
	UNUSED(misc);

	struct StringSpan name = CONTEXT(struct StringSpan);
	String_Encode(name.data, name.size, pokemon->nickname, POKEMON_NICKNAME_SIZE);
}

static void
Actions_TrainerName(struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc, const void* context)
{
	UNUSED(misc);

	struct StringSpan name = CONTEXT(struct StringSpan);
	String_Encode(name.data, name.size, pokemon->trainerName, POKEMON_OT_NAME_SIZE);
}

static void
Actions_TrainerGender(struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc, const void* context)
{
	UNUSED(pokemon);

	misc->origin.gender = CONTEXT(bool);
}

static void
Actions_MetLocation(struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc, const void* context)
{
	UNUSED(misc);

	pokemon->data.misc.location = CONTEXT(uint8_t);
}

static void
Actions_HeldItem(struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc, const void* context)
{
	UNUSED(misc);

	pokemon->data.growth.item = CONTEXT(uint16_t);
}

static void
Actions_Ball(struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc, const void* context)
{
	UNUSED(pokemon);

	misc->origin.ball = CONTEXT(uint8_t);
}

struct ActionInfo
{
	const char* name;
	FnAction* func;
	FnParseContext* parseContext;
};

struct ActionInfo const GActions[] = {
	{ "nickname", Actions_Nickname, ParseContext_StringSpan },
	{ "trainer-name", Actions_TrainerName, ParseContext_StringSpan },
	{ "trainer-gender", Actions_TrainerGender, ParseContext_Gender },
	{ "met-location", Actions_MetLocation, ParseContext_uint8 },
	{ "held-item", Actions_HeldItem, ParseContext_uint16 },
	{ "ball", Actions_Ball, ParseContext_uint8 },
};

struct ProgramArguments
{
	const char* file;

	size_t filterCount;
	struct Filter filters[32];

	size_t actionCount;
	struct Action actions[32];
};

typedef size_t FnParseCommand(struct ProgramArguments* arguments, size_t argc, const char** argv);

struct CommandInfo
{
	const char* name;
	FnParseCommand* parse;
};

static size_t
Commands_Where(struct ProgramArguments* arguments, size_t argc, const char** argv)
{
	if (argc < 2)
		return 0;

	bool expect = true;
	if (strcmp(argv[0], "not") == 0)
	{
		expect = false;
		if (argc < 3)
			return 0;
		++argv;
	}

	const char* key = argv[0];
	for (size_t i = 0, c = ARRAY_SIZE(GFilters); i < c; ++i)
	{
		const struct FilterInfo* info = &GFilters[i];

		if (strcmp(info->name, key) == 0)
		{
			size_t index = arguments->filterCount;
			if (index == ARRAY_SIZE(arguments->filters))
				return 0;
			struct Filter* filter = &arguments->filters[index];
			++arguments->filterCount;

			const struct StringSpan val = StringSpan_FromCString(argv[1]);
			if (!info->parseContext(&filter->context, val))
				return 0;
			filter->func = info->func;
			filter->expect = expect;
			return expect ? 2 : 3;
		}
	}

	return 0;
}

static size_t
Commands_Set(struct ProgramArguments* arguments, size_t argc, const char** argv)
{
	if (argc < 2)
		return 0;

	const char* key = argv[0];
	for (size_t i = 0, c = ARRAY_SIZE(GActions); i < c; ++i)
	{
		const struct ActionInfo* info = &GActions[i];

		if (strcmp(info->name, key) == 0)
		{
			size_t index = arguments->actionCount;
			if (index == ARRAY_SIZE(arguments->actions))
				return 0;
			struct Action* action = &arguments->actions[index];
			++arguments->actionCount;

			const struct StringSpan val = StringSpan_FromCString(argv[1]);
			if (!info->parseContext(&action->context, val))
				return 0;
			action->func = info->func;
			return 2;
		}
	}

	return 0;
}

static const struct CommandInfo GCommands[] = {
	{ "where", Commands_Where },
	{ "set", Commands_Set },
};

static bool
ProgramArguments_Parse(struct ProgramArguments* arguments, size_t argc, const char** argv)
{
	if (argc == 0)
		return false;

	arguments->file = argv[0];
	arguments->filterCount = 0;
	arguments->actionCount = 0;

	for (size_t i = 1; i < argc;)
	{
		const char* name = argv[i++];

		const struct CommandInfo* info = GCommands;
		for (const struct CommandInfo* last = info + ARRAY_SIZE(GCommands);;)
		{
			if (strcmp(info->name, name) == 0)
				break;

			if (++info == last)
				return false;
		}

		size_t count = info->parse(arguments, argc - i, argv + i);

		if (count == 0)
			return false;

		i += count;
	}

	return true;
}

static bool
ProgramArguments_Filter(const struct ProgramArguments* arguments, const struct Pokemon* pokemon, const struct Pokemon_Misc_Unpacked* misc, size_t index)
{
	for (size_t i = 0, c = arguments->filterCount; i < c; ++i)
		if (!Filter_Invoke(&arguments->filters[i], pokemon, misc, index))
			return false;
	return true;
}

static void
ProgramArguments_Mutate(const struct ProgramArguments* arguments, struct Pokemon* pokemon, struct Pokemon_Misc_Unpacked* misc)
{
	for (size_t i = 0, c = arguments->actionCount; i < c; ++i)
		Action_Invoke(&arguments->actions[i], pokemon, misc);
}

int
main(int argc, const char** argv)
{
	struct ProgramArguments args;
	if (!ProgramArguments_Parse(&args, argc - 1, argv + 1))
		return 1;

	struct Battery battery;
	if (!Battery_Load(&battery, args.file))
		return 1;

	struct Save* save;
	if (!Battery_GetCurrentSave(&battery, &save))
		return 1;

	struct Section* sections[SECTION_COUNT];
	if (!Save_GetSections(save, sections))
		return 1;

	struct PokemonStorage storage;
	if (!PokemonStorage_Load(&storage, sections))
		return 1;

	bool mutate = args.actionCount > 0;
	for (size_t i = 0; i < STORAGE_BOX_COUNT * STORAGE_BOX_SIZE; ++i)
	{
		struct Pokemon* pokemon = &storage.pokemon[i];

		if (!Pokemon_Exists(pokemon))
			continue;

		Pokemon_Decrypt(pokemon);
		if (Pokemon_CalculateChecksum(pokemon) != pokemon->checksum)
			return 1;
		Pokemon_Unscramble(pokemon);

		struct Pokemon_Misc_Unpacked misc;
		Pokemon_Misc_Unpack(&pokemon->data.misc, &misc);

		char nickname[32];
		if (misc.values.egg && memcmp(pokemon->nickname, "\x60\x6F\x8B\xFF", 4) == 0)
			strcpy(nickname, "@EGG");
		else String_Decode(pokemon->nickname, POKEMON_NICKNAME_SIZE, nickname, sizeof(nickname));

		char trainerName[32];
		String_Decode(pokemon->trainerName, POKEMON_OT_NAME_SIZE, trainerName, sizeof(trainerName));

		if (ProgramArguments_Filter(&args, pokemon, &misc, i))
		{
			size_t box = i / STORAGE_BOX_SIZE + 1;
			size_t slot = i % STORAGE_BOX_SIZE + 1;

			const struct PokemonInfo* info = &GPokemon[pokemon->data.growth.species];

			printf("%02u/%02u: %03u %-" PP_STR(POKEMON_NICKNAME_SIZE) "s %-" PP_STR(POKEMON_NICKNAME_SIZE) "s from %05u %c %-" PP_STR(POKEMON_OT_NAME_SIZE) "s",
				(unsigned)box, (unsigned)slot, info->index, info->name, nickname, pokemon->trainerPublic, misc.origin.gender ? 'F' : 'M', trainerName);

			putchar('\n');

			if (mutate)
			{
				ProgramArguments_Mutate(&args, pokemon, &misc);
				Pokemon_Misc_Pack(&pokemon->data.misc, &misc);
			}
		}

		Pokemon_Scramble(pokemon);
		pokemon->checksum = Pokemon_CalculateChecksum(pokemon);
		Pokemon_Encrypt(pokemon);
	}

	if (mutate)
	{
		if (!PokemonStorage_Save(&storage, sections))
			return 1;

		if (!Battery_Save(&battery, args.file))
			return 1;
	}

	return 0;
}
