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
