#include "qc/config/config.h"

namespace qc {

ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
    auto it = getVars().find(name);
    return it == getVars().end() ? nullptr : it->second;
}

void Config::TraversalYaml(const std::string& prefix, const YAML::Node& node, std::list<std::pair<std::string, const YAML::Node>>& nodes) {
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._") != std::string::npos) {
        LOG_ERROR(ROOT_LOG()) << "TraversalYaml prefix contains invalid characters: " << prefix;
        return;
    }
    nodes.emplace_back(prefix, node);
    if (node.IsMap()) {
        for (const auto& it : node) {
            const std::string& key = it.first.Scalar();
            TraversalYaml(prefix.empty() ? key : prefix + "." + key, it.second, nodes);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node>> nodes;
    TraversalYaml("", root, nodes);

    for (auto& it : nodes) {
        std::string key = it.first;
        if (key.empty()) {
            continue;
        }

        ConfigVarBase::ptr var = LookupBase(key);
        if (var) {
            var->fromNode(it.second);
        }
    }
}

}