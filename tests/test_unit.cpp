#include "coglux/coglux.hpp"
#include <iostream>
#include <cstring>
#include <cassert>

// Simple test framework
#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAILED: " << msg << std::endl; \
            return 1; \
        } \
    } while(0)

#define TEST_PASS(name) \
    std::cout << "PASSED: " << name << std::endl

// Test functions
int test_version() {
    std::string ver = coglux::version();
    TEST_ASSERT(!ver.empty(), "Version should not be empty");
    TEST_ASSERT(ver.find('.') != std::string::npos, "Version should contain dots");
    TEST_PASS("version");
    return 0;
}

int test_node() {
    std::unique_ptr<coglux::SimpleCogNode> node(new coglux::SimpleCogNode("test_node"));
    TEST_ASSERT(node != nullptr, "Node should be created");
    TEST_ASSERT(node->name() == "test_node", "Node name should match");
    TEST_ASSERT(node->process(), "Node should process successfully");
    TEST_PASS("node");
    return 0;
}

int test_graph() {
    coglux::CogGraph graph;
    TEST_ASSERT(graph.node_count() == 0, "Empty graph should have 0 nodes");
    
    graph.add_node(std::unique_ptr<coglux::CogNode>(new coglux::SimpleCogNode("node1")));
    TEST_ASSERT(graph.node_count() == 1, "Graph should have 1 node");
    
    graph.add_node(std::unique_ptr<coglux::CogNode>(new coglux::SimpleCogNode("node2")));
    graph.add_node(std::unique_ptr<coglux::CogNode>(new coglux::SimpleCogNode("node3")));
    TEST_ASSERT(graph.node_count() == 3, "Graph should have 3 nodes");
    
    TEST_ASSERT(graph.process_all(), "Graph should process all nodes");
    TEST_PASS("graph");
    return 0;
}

int test_init_shutdown() {
    TEST_ASSERT(coglux::initialize(), "Initialize should succeed");
    coglux::shutdown();
    TEST_ASSERT(coglux::initialize(), "Re-initialize should succeed");
    coglux::shutdown();
    TEST_PASS("init_shutdown");
    return 0;
}

int run_all_tests() {
    int failures = 0;
    
    std::cout << "Running all CogLux unit tests" << std::endl;
    std::cout << "=============================" << std::endl;
    
    failures += test_version();
    failures += test_node();
    failures += test_graph();
    failures += test_init_shutdown();
    
    std::cout << "=============================" << std::endl;
    if (failures == 0) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << failures << " test(s) failed" << std::endl;
    }
    
    return failures;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return run_all_tests();
    }
    
    const char* test_name = argv[1];
    
    if (strcmp(test_name, "version") == 0) {
        return test_version();
    } else if (strcmp(test_name, "node") == 0) {
        return test_node();
    } else if (strcmp(test_name, "graph") == 0) {
        return test_graph();
    } else if (strcmp(test_name, "init_shutdown") == 0) {
        return test_init_shutdown();
    } else {
        std::cerr << "Unknown test: " << test_name << std::endl;
        return 1;
    }
}
