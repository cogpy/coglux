#include "coglux/coglux.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "CogLux Basic Test" << std::endl;
    std::cout << "=================" << std::endl;
    
    // Test version
    std::string ver = coglux::version();
    std::cout << "Version: " << ver << std::endl;
    assert(!ver.empty());
    
    // Test initialization
    bool init = coglux::initialize();
    std::cout << "Initialize: " << (init ? "OK" : "FAILED") << std::endl;
    assert(init);
    
    // Test node creation
    std::unique_ptr<coglux::SimpleCogNode> node(new coglux::SimpleCogNode("test_node"));
    std::cout << "Node name: " << node->name() << std::endl;
    assert(node->name() == "test_node");
    
    // Test node processing
    bool processed = node->process();
    std::cout << "Process: " << (processed ? "OK" : "FAILED") << std::endl;
    assert(processed);
    
    // Test graph
    coglux::CogGraph graph;
    graph.add_node(std::unique_ptr<coglux::CogNode>(new coglux::SimpleCogNode("node1")));
    graph.add_node(std::unique_ptr<coglux::CogNode>(new coglux::SimpleCogNode("node2")));
    std::cout << "Graph nodes: " << graph.node_count() << std::endl;
    assert(graph.node_count() == 2);
    
    bool graph_processed = graph.process_all();
    std::cout << "Graph process: " << (graph_processed ? "OK" : "FAILED") << std::endl;
    assert(graph_processed);
    
    // Cleanup
    coglux::shutdown();
    
    std::cout << "=================" << std::endl;
    std::cout << "All tests passed!" << std::endl;
    
    return 0;
}
