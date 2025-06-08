#include <fstream>
#include "qc/log/log_module.h"
#include "qc/config/config_module.h"
#include "qc/macro.h"

// class Person {
//   public:
//     std::string m_name;
//     int m_age;
//     bool m_sex;

//     std::string toString() const {
//         std::stringstream ss;
//         ss << "Person(name: " << m_name << ", age: " << m_age
//         << ", sex: " << m_sex << ")";
//         return ss.str();
//     }

//     bool operator==(const Person& other) const {
//         return m_name == other.m_name && m_age == other.m_age && m_sex == other.m_sex;
//     }
// };

// namespace YAML {
// template<>
// struct convert<Person> {
//     static Node encode(const Person& person) {
//         Node node;
//         node["name"] = person.m_name;
//         node["age"] = person.m_age;
//         node["sex"] = person.m_sex;
//         return node;
//     }
//     static bool decode(const Node& node, Person& person) {
//         if (!node.IsMap() || !node["name"] || !node["age"] || !node["sex"]) {
//             return false;
//         }
//         person.m_name = node["name"].as<std::string>();
//         person.m_age = node["age"].as<int>();
//         person.m_sex = node["sex"].as<bool>();
//         return true;
//     }
// };
// }

using namespace qc;


// static qc::LogIniter _log_init;
// ConfigVar<std::unordered_map<std::string, LogConfig>>::ptr g_log_config = 
//     Config::Create("logs", std::unordered_map<std::string, LogConfig>{}, "this is a system log config");


void test_log_config() {
    // LOG_INFO(GET_LOGGER("system")) << "This is a test log message for system";
    // LOG_INFO(ROOT_LOG()) << "Before:" << g_log_config->getValue().size();
    LOG_INFO(GET_LOGGER("root1")) << "This is a test log message for root1 before config load";
    YAML::Node root = YAML::LoadFile("./conf/log.yml");
    Config::LoadFromYaml(root);
    LOG_INFO(ROOT_LOG()) << "xxx";
    // LOG_INFO(GET_LOGGER("root1")) << "This is a test log message for root1 after config load";
    LOG_INFO(GET_LOGGER("system")) << "This is a test log message for system after config load";
    // LOG_INFO(ROOT_LOG()) << "After:" << g_log_config->getValue().size();
    // LOG_INFO(ROOT_LOG()) << g_log_config->toString();
    // Config::Visit([](ConfigVarBase::ptr var) {
    //     LOG_INFO(ROOT_LOG()) << "name:" << var->getName() << " description:" << var->getDescription()
    //               << " value:" << var->toString();
    // });
    // YAML::Node node = Config::GetConfig();
    // std::ofstream fout("output.yml");
    // fout << node;
    // fout.close();
}
// ConfigVar<Person>::ptr g_person = 
//     Config::Create("class.person", Person{"John Doe", 30, true}, "this is a system person config");
// ConfigVar<std::map<std::string, Person>>::ptr g_person_map = 
//     Config::Create("class.map", std::map<std::string, Person>{{"person1", {"Alice", 25, false}}, {"person2", {"Bob", 28, true}}}, "this is a system person map config");
// ConfigVar<std::map<std::string, std::vector<Person>>>::ptr g_person_vector_map = 
//     Config::Create("class.vector_map", std::map<std::string, std::vector<Person>>{{"group1", {{"Charlie", 22, true}, {"Diana", 24, false}}}}, "this is a system person vector map config");

// ConfigVar<int>::ptr g_int_value_config = 
//     Config::Create("system.port", (int)8080, "this is a system port");
// ConfigVar<int>::ptr g_int_value_config1 = 
//     Config::Create("system.port", (int)1010, "this is a system port");
// ConfigVar<float>::ptr g_int_value_config2 = 
//     Config::Create("system.port", (float)2020, "this is a system port");
// ConfigVar<float>::ptr g_float_value_config = 
//     Config::Create("system.value", (float)3.14, "this is a system float value");
// ConfigVar<std::vector<int>>::ptr g_vector_int_config = 
//     Config::Create("system.vector_int", std::vector<int>{1, 2, 3}, "this is a system vector int value");
// ConfigVar<std::list<int>>::ptr g_list_int_config = 
//     Config::Create("system.list_int", std::list<int>{4, 5, 6}, "this is a system list int value");
// ConfigVar<std::set<int>>::ptr g_set_int_config =
//     Config::Create("system.set_int", std::set<int>{7, 8, 9}, "this is a system set int value");
// ConfigVar<std::unordered_set<int>>::ptr g_unordered_set_int_config =
//     Config::Create("system.unordered_set_int", std::unordered_set<int>{10, 11, 12}, "this is a system unordered set int value");
// ConfigVar<std::map<std::string, int>>::ptr g_map_string_int_config =
//     Config::Create("system.map_string_int", std::map<std::string, int>{{"key1", 13}, {"key2", 14}}, "this is a system map string int value");
// ConfigVar<std::unordered_map<std::string, int>>::ptr g_unordered_map_string_int_config =
//     Config::Create("system.unordered_map_string_int", std::unordered_map<std::string, int>{{"key3", 15}, {"key4", 16}}, "this is a system unordered map string int value");

// void test_class() {
//     g_person->addListener(17, [](const Person& old_value, const Person& new_value) {
//         LOG_INFO(ROOT_LOG()) << "Person Changed: "
//                              << "Old: " << old_value.toString()
//                              << ", New: " << new_value.toString();
//     });
//     LOG_INFO(ROOT_LOG()) << "Before:" << g_person->getValue().toString() << g_person->toString();
// #define XX(g_var, prefix) \
//     { \
//         auto _var = g_var->getValue(); \
//         for (const auto& [k, v] : _var) { \
//             LOG_INFO(ROOT_LOG()) << #prefix ":" << k << " - " << v.toString(); \
//         } \
//         LOG_INFO(ROOT_LOG()) << #prefix ":" <<  g_var->toString(); \
//     }
//     XX(g_person_map, Before);
//     LOG_INFO(ROOT_LOG()) << "Before:" << g_person_vector_map->toString();
//     YAML::Node root = YAML::LoadFile("./conf/test.yml");
//     Config::LoadFromYaml(root);
//     LOG_INFO(ROOT_LOG()) << "After:" << g_person->getValue().toString() << g_person->toString();
//     XX(g_person_map, After);
//     LOG_INFO(ROOT_LOG()) << "After:" << g_person_vector_map->toString();
// #undef XX
// }

// void print_yaml(const YAML::Node& node, int level) {
//     if (node.IsScalar()) {
//         LOG_INFO(ROOT_LOG()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
//     } else if (node.IsNull()) {
//         LOG_INFO(ROOT_LOG()) << std::string(level * 4, ' ') << "null - " << node.Type() << " - " << level;
//     } else if (node.IsMap()) {
//         for (const auto& it : node) {
//             LOG_INFO(ROOT_LOG()) << std::string(level * 4, ' ') << it.first.Scalar() << " - " << it.second.Type() << " - " << level;
//             print_yaml(it.second, level + 1);
//         }
//     } else if (node.IsSequence()) {
//         for (size_t i = 0; i < node.size(); ++i) {
//             LOG_INFO(ROOT_LOG()) << std::string(level * 4, ' ') << "[" << i << "]"  << " - " << node[i].Type() << " - " << level;
//             print_yaml(node[i], level + 1);
//         }
//     } else {
//         LOG_INFO(ROOT_LOG()) << std::string(level * 4, ' ') << "unknown type - " << node.Type() << " - " << level;
//     }
// }

// void test_yaml() {
//     YAML::Node root = YAML::LoadFile("./conf/test.yml");

//     // LOG_INFO(ROOT_LOG()) << "root:" << root;
//     print_yaml(root, 0);
// }

// void load_yaml() {
//     YAML::Node root = YAML::LoadFile("./conf/test.yml");
//     if (!root) {
//         LOG_ERROR(ROOT_LOG()) << "Failed to load YAML file";
//         return;
//     }

//     Config::LoadFromYaml(root);
//     LOG_INFO(ROOT_LOG()) << "YAML configuration loaded successfully";
// }

// void test_cast_yaml() {
// #define XX(g_var, name, prefix) \
//     {\
//         auto _var = g_var->getValue(); \
//         for (auto var : _var) { \
//             LOG_INFO(ROOT_LOG()) << #prefix " " #name ":" << var; \
//         } \
//         LOG_INFO(ROOT_LOG()) << #prefix " " #name " toString:" << g_var->toString(); \
//     }
// #define XX_MAP(g_var, name, prefix) \
//     {\
//         auto _var = g_var->getValue(); \
//         for (const auto& [k, v] : _var) { \
//             LOG_INFO(ROOT_LOG()) << #prefix " " #name ": {" << k << " - " << v << "}"; \
//         } \
//         LOG_INFO(ROOT_LOG()) << #prefix " " #name " toString:" << g_var->toString(); \
//     }
//     XX(g_vector_int_config, vector_int, "Before");
//     XX(g_vector_int_config1, vector_int1, "Before");
//     XX(g_list_int_config, list_int, "Before");
//     XX(g_set_int_config, set_int, "Before");
//     XX(g_unordered_set_int_config, unordered_set_int, "Before");
//     XX_MAP(g_map_string_int_config, map_string_int, "Before");
//     XX_MAP(g_unordered_map_string_int_config, unordered_map_string_int, "Before");
//     // test_yaml();
//     load_yaml();
//     XX(g_vector_int_config, vector_int, "After");
//     XX(g_vector_int_config1, vector_int1, "After");
//     XX(g_list_int_config, list_int, "After");
//     XX(g_set_int_config, set_int, "After");
//     XX(g_unordered_set_int_config, unordered_set_int, "After");
//     XX_MAP(g_map_string_int_config, map_string_int, "After");
//     XX_MAP(g_unordered_map_string_int_config, unordered_map_string_int, "After");
// #undef XX
// #undef XX_MAP
// }

int main(int argc, char** argv) {
    // test_yaml();
    // test_cast_yaml();
    // test_class();
    test_log_config();
    return 0;
}