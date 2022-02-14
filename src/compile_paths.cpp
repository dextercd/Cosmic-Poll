#include <filesystem>

namespace fs = std::filesystem;

extern fs::path const default_db_location = fs::path{COSMIC_DEFAULT_DB_LOCATION};
