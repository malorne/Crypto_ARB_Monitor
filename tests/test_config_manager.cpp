#include <gtest/gtest.h>
#include "config/ConfigManager.h"
#include "exceptions/Exceptions.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

static std::string tmp_conf(const std::string& body) {
    auto p = fs::temp_directory_path() / "am_cfg_test.conf";
    std::ofstream f(p); f << body;
    return p.string();
}

TEST(ConfigManager, ThrowsWhenFileMissing) {
    am::ConfigManager cfg("/no/such/file.conf");
    EXPECT_THROW(cfg.load(), am::ConfigError);
}

TEST(ConfigManager, ParsesKeyValue) {
    auto p = tmp_conf("foo = bar\nbaz = 3.14\n");
    am::ConfigManager cfg(p); cfg.load();
    EXPECT_EQ(*cfg.get_string("foo"), "bar");
    EXPECT_DOUBLE_EQ(*cfg.get_double("baz"), 3.14);
}

TEST(ConfigManager, IgnoresComments) {
    auto p = tmp_conf("# comment\nkey = val\n");
    am::ConfigManager cfg(p); cfg.load();
    EXPECT_EQ(*cfg.get_string("key"), "val");
    EXPECT_FALSE(cfg.get_string("# comment").has_value());
}

TEST(ConfigManager, ParsesBool) {
    auto p = tmp_conf("a = true\nb = false\n");
    am::ConfigManager cfg(p); cfg.load();
    EXPECT_EQ(*cfg.get_bool("a"), true);
    EXPECT_EQ(*cfg.get_bool("b"), false);
}

TEST(ConfigManager, ThrowsOnBadDouble) {
    auto p = tmp_conf("x = notanumber\n");
    am::ConfigManager cfg(p); cfg.load();
    EXPECT_THROW(cfg.get_double("x"), am::ConfigError);
}
