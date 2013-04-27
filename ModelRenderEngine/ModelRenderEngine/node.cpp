//
//  node.cpp
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#include "node.h"

namespace mre {
    node::node(const std::string &name, int index)
        : name(name)
        , index(index)
    {
        
    }
    
    node::node(const mre::node &node)
        : name(node.name)
        , index(node.index) {
        
    }
    
    const std::string& node::node_name() const {
        return name;
    }
    
    bool node::operator==(const mre::node &node) const {
        if (name != node.name) {
            return false;
        }
        if (index != node.index) {
            return false;
        }
        return true;
    }
    
    bool node::operator!=(const mre::node &node) const {
        return !(*this == node);
    }
}