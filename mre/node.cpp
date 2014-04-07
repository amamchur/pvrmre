//
//  Copyright 2013-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

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