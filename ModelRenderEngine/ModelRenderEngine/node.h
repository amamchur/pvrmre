//
//  node.h
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#ifndef PVRMRE_NODE_H
#define PVRMRE_NODE_H

#include <string>

namespace mre {
    class node {
    private:
        std::string name;
        int index;
    public:
        node(const std::string &name, int index);
        node(const mre::node &node);
        
        bool operator ==(const mre::node &node) const;
        bool operator !=(const mre::node &node) const;
        
        const std::string& node_name() const;
    };
}

#endif
