#ifndef QC_CONFIG_H
#define QC_CONFIG_H

#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <functional>
#include <yaml-cpp/yaml.h>
#include "qc/macro.h"
#include "qc/thread/mutex.h"

namespace YAML {

template<typename T>
struct convert<std::vector<T>> {
    static Node encode(const std::vector<T>& vec) {
        Node node;
        for (const auto& item : vec) {
            node.push_back(convert<T>::encode(item));
        }
        return node;
    }
    static bool decode(const Node& node, std::vector<T>& vec) {
        if (!node.IsSequence()) {
            return false;
        }
        for (const auto& item : node) {
            vec.push_back(item.as<T>());
        }
        return true;
    }
};

template<typename T>
struct convert<std::list<T>> {
    static Node encode(const std::list<T>& lst) {
        Node node;
        for (const auto& item : lst) {
            node.push_back(convert<T>::encode(item));
        }
        return node;
    }
    static bool decode(const Node& node, std::list<T>& lst) {
        if (!node.IsSequence()) {
            return false;
        }
        for (const auto& item : node) {
            lst.push_back(item.as<T>());
        }
        return true;
    }
};

template<typename T>
struct convert<std::set<T>> {
    static Node encode(const std::set<T>& set) {
        Node node;
        for (const auto& item : set) {
            node.push_back(convert<T>::encode(item));
        }
        return node;
    }
    static bool decode(const Node& node, std::set<T>& set) {
        if (!node.IsSequence()) {
            return false;
        }
        for (const auto& item : node) {
            set.insert(item.as<T>());
        }
        return true;
    }
};

template<typename T>
struct convert<std::map<std::string, T>> {
    static Node encode(const std::map<std::string, T>& map) {
        Node node;
        for (const auto& [key, value] : map) {
            node[key] = convert<T>::encode(value);
        }
        return node;
    }
    static bool decode(const Node& node, std::map<std::string, T>& map) {
        if (!node.IsMap()) {
            return false;
        }
        for (const auto& pair : node) {
            map.emplace(pair.first.Scalar(), pair.second.as<T>());
        }
        return true;
    }
};

template<typename T>
struct convert<std::unordered_set<T>> {
    static Node encode(const std::unordered_set<T>& set) {
        Node node;
        for (const auto& item : set) {
            node.push_back(convert<T>::encode(item));
        }
        return node;
    }
    static bool decode(const Node& node, std::unordered_set<T>& set) {
        if (!node.IsSequence()) {
            return false;
        }
        for (const auto& item : node) {
            set.insert(item.as<T>());
        }
        return true;
    }
};

template<typename T>
struct convert<std::unordered_map<std::string, T>> {
    static Node encode(const std::unordered_map<std::string, T>& map) {
        Node node;
        for (const auto& [key, value] : map) {
            node[key] = convert<T>::encode(value);
        }
        return node;
    }
    static bool decode(const Node& node, std::unordered_map<std::string, T>& map) {
        if (!node.IsMap()) {
            return false;
        }
        for (const auto& pair : node) {
            map.emplace(pair.first.Scalar(), pair.second.as<T>());
        }
        return true;
    }
};

}

namespace qc {

class ConfigVarBase {
  public:
    using ptr = std::shared_ptr<ConfigVarBase>;
    
    ConfigVarBase(const std::string& name, const std::string& description = "") 
        : m_name(name), m_description(description) {}

    virtual ~ConfigVarBase() = default;

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

    virtual std::string toString() const = 0;
    virtual void fromString(const std::string& val) = 0;
    virtual YAML::Node toNode() const = 0;
    virtual void fromNode(const YAML::Node& node) = 0;
    virtual std::string getTypeName() const = 0;
  protected:
    std::string m_name;
    std::string m_description;
};

template<class T>
class ConfigVar : public ConfigVarBase {
  public:
    using ptr = std::shared_ptr<ConfigVar>;
    using callback = std::function<void(const T& old_val, const T& new_val)>;
    using RWMutex = qc::thread::RWMutex;

    ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
        : ConfigVarBase(name, description), m_value(default_value) {}

    std::string toString() const override {
        RWMutex::RLock lock(m_mutex);
        try {
            YAML::Node node = YAML::convert<T>::encode(m_value);
            std::stringstream ss;
            ss << node;
            return ss.str();
        } catch (std::exception& e) {
            LOG_ERROR(ROOT_LOG()) << "ConfigVar::toString exception: " 
                << e.what() << " convert: " << typeid(T).name() 
                << " to string, name: " << m_name;
        }
        return "";
    }

    void fromString(const std::string& val) override {
        RWMutex::WLock lock(m_mutex);
        try {
            YAML::Node node = YAML::Load(val);
            setValue(node.as<T>());
            return;
        } catch (std::exception& e) {
            LOG_ERROR(ROOT_LOG()) << "ConfigVar::fromString exception: " 
                << e.what() << " convert: " << val 
                << " to type: " << typeid(T).name() 
                << ", name: " << m_name;
        }
    }

    YAML::Node toNode() const override {
        RWMutex::RLock lock(m_mutex);
        try {
            return YAML::Node(m_value);
        } catch (std::exception& e) {
            LOG_ERROR(ROOT_LOG()) << "ConfigVar::toNode exception: " 
                << e.what() << " convert: " << typeid(T).name() 
                << " to string, name: " << m_name;
        }
        return YAML::Node();
    }

    void fromNode(const YAML::Node& node) override {
        RWMutex::WLock lock(m_mutex);
        try {
            setValue(node.as<T>());
            return;
        } catch (std::exception& e) {
            LOG_ERROR(ROOT_LOG()) << "ConfigVar::fromNode exception: " 
                << e.what() << " convert: " << node 
                << " to type: " << typeid(T).name() 
                << ", name: " << m_name;
        }
    }

    const T& getValue() const { 
        return m_value; 
    }
    void setValue(const T& value) {
        if (m_value == value) {
            return;
        }
        if (m_cb) {
            m_cb(m_value, value);
        }
        m_value = value;
    }
    
    std::string getTypeName() const override { return typeid(T).name(); }

    void setListener(const callback& cb) {
        RWMutex::WLock lock(m_mutex);
        m_cb = cb;
    }

    void removeListener() {
        RWMutex::WLock lock(m_mutex);
        m_cb = nullptr;
    }

    bool hasListener() const {
        RWMutex::RLock lock(m_mutex);
        return m_cb != nullptr;
    }
  private:
    mutable RWMutex m_mutex;
    T m_value;
    callback m_cb;
};

class Config {
  public:
    using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;
    using RWMutex = qc::thread::RWMutex;

    template<class T>
    static typename ConfigVar<T>::ptr Create(const std::string& name,
                                              const T& default_value,
                                              const std::string& description = "") {
        if (name.empty()) {
            LOG_ERROR(ROOT_LOG()) << "ConfigVar name cannot be empty";
            return nullptr;
        }
        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._") != std::string::npos) {
            LOG_ERROR(ROOT_LOG()) << "ConfigVar name contains invalid characters: " << name;
            return nullptr;
        }
        
        auto it = getVars().find(name);
        if (it != getVars().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (tmp) {
                LOG_ERROR(ROOT_LOG()) << "ConfigVar already exists: " 
                    << name << ", type: " << typeid(T).name();
                return tmp;
            } else {
                LOG_ERROR(ROOT_LOG()) << "ConfigVar type mismatch: " 
                    << name << ", expected: " << typeid(T).name() 
                    << ", actual: " << it->second->getTypeName()
                    << ", value: " << it->second->toString();
                return nullptr;
            }
        }
        LOG_INFO(ROOT_LOG()) << "Create ConfigVar: " << name << ", description: " << description;
        auto var = std::make_shared<ConfigVar<T>>(name, default_value, description);
        getVars()[name] = var;
        return var;
    }

    static const YAML::Node GetConfig();
    static void LoadFromYaml(const YAML::Node& root);
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
  private:
    static RWMutex& getMutex() {
        static RWMutex s_mutex;
        return s_mutex;
    }

    static ConfigVarMap& getVars() {
        static ConfigVarMap m_vars;
        return m_vars;
    }

    static ConfigVarBase::ptr LookupBase(const std::string& name);

    static void TraversalYaml(const std::string& prefix, const YAML::Node& node, std::list<std::pair<std::string, const YAML::Node>>& list);
};

}

#endif