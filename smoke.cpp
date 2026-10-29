#include <meta>
#include <print>

struct AddOrder { char type; };

int main() {
std::println("{}", std::meta::identifier_of(^^AddOrder));
}
