#include "coglux/coglux.hpp"
#include <iostream>

namespace coglux {

// SimpleCogNode implementation
SimpleCogNode::SimpleCogNode(const std::string& name) : name_(name) {}

std::string SimpleCogNode::name() const {
    return name_;
}

bool SimpleCogNode::process() {
    // Simple processing - can be extended
    return true;
}

// CogGraph implementation
void CogGraph::add_node(std::unique_ptr<CogNode> node) {
    nodes_.push_back(std::move(node));
}

size_t CogGraph::node_count() const {
    return nodes_.size();
}

bool CogGraph::process_all() {
    for (auto& node : nodes_) {
        if (!node->process()) {
            return false;
        }
    }
    return true;
}

// Library functions
bool initialize() {
    // Initialize library resources
    return true;
}

void shutdown() {
    // Clean up library resources
}

std::string version() {
    return Version::to_string();
}

} // namespace coglux
