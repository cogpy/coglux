#pragma once

#include <string>
#include <vector>
#include <memory>

namespace coglux {

/**
 * @brief CogLux library version information
 */
struct Version {
    static constexpr int major = 0;
    static constexpr int minor = 1;
    static constexpr int patch = 0;
    
    static std::string to_string() {
        return std::to_string(major) + "." + 
               std::to_string(minor) + "." + 
               std::to_string(patch);
    }
};

/**
 * @brief Base class for cognitive computing nodes
 */
class CogNode {
public:
    CogNode() = default;
    virtual ~CogNode() = default;
    
    virtual std::string name() const = 0;
    virtual bool process() = 0;
};

/**
 * @brief Simple cognitive node implementation
 */
class SimpleCogNode : public CogNode {
public:
    explicit SimpleCogNode(const std::string& name);
    
    std::string name() const override;
    bool process() override;
    
private:
    std::string name_;
};

/**
 * @brief Cognitive graph for managing nodes
 */
class CogGraph {
public:
    CogGraph() = default;
    ~CogGraph() = default;
    
    void add_node(std::unique_ptr<CogNode> node);
    size_t node_count() const;
    bool process_all();
    
private:
    std::vector<std::unique_ptr<CogNode>> nodes_;
};

/**
 * @brief Initialize the CogLux library
 * @return true if initialization succeeded
 */
bool initialize();

/**
 * @brief Shutdown the CogLux library
 */
void shutdown();

/**
 * @brief Get the library version string
 */
std::string version();

} // namespace coglux
