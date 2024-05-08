#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <system_error>

#include "address_book.hpp"
#include "synchronization.hpp"

TEST_CASE("entries can be added and removed") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("Jane Doe"));
	ab.remove_entry("Jane Doe");
	CHECK_FALSE(ab.has_entry("Jane Doe"));
}

TEST_CASE("entry names are case-insensitive") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("jane doe"));
	ab.remove_entry("Jane Doe");
}

TEST_CASE("entry names are are between 1 and 100 characters long") {
	address_book ab;
	CHECK_THROWS(ab.add_entry(""));
	std::string name(101,'a');
	CHECK_THROWS(ab.add_entry(name));

	name = std::string(100,'a');
	ab.add_entry(name);
	CHECK(ab.has_entry(name));

	ab.add_entry("a");
	CHECK(ab.has_entry("a"));
}

TEST_CASE("entries are sorted alphabetically") {
	address_book ab;
	ab.add_entry("Jane Doe");
	ab.add_entry("Zoe Doe");
	ab.add_entry("Anne Doe");
	auto entries = ab.get_entries();
	REQUIRE(entries.size()==3);

	CHECK(entries[0] == "Anne Doe");
	CHECK(entries[1] == "Jane Doe");
	CHECK(entries[2] == "Zoe Doe");
}

TEST_CASE("check file based syncronisation provider") {
	address_book ab;
	file_synchronization_provider f_provider{"db"};
	ab.synchronize(f_provider);
	CHECK_FALSE(ab.has_entry("jane"));
	ab.add_entry("jane");
	ab.synchronize(f_provider);
	CHECK(ab.has_entry("jane"));
	ab.remove_entry("jane");
	CHECK_FALSE(ab.has_entry("jane"));
	ab.synchronize(f_provider);
	CHECK(ab.has_entry("jane"));
}