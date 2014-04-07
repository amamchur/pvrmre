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

#ifndef PVRMRE_MODEL_H
#define PVRMRE_MODEL_H

#include <list>
#include <string>
#include <map>

#include "OGLES2Tools.h"
#include "light.h"
#include "camera.h"

namespace mre {
    class node;
    class effect_overrides;
    class light;
    
    class model {
    private:
        typedef std::map<std::string, CPVRTPFXEffect*> effects_map;
        typedef std::map<int, mre::effect_overrides> nodes_overrides_map;
        
        CPVRTModelPOD *pod_model;
        std::string pod_directory;
        
        GLuint *vbos;
        GLuint *indexVbo;
        
        PVRTMat4 world;
        
        effects_map effects;
        nodes_overrides_map nodes_overrides;

        int selected_node_index;
        
        void load_vbo();
        void load_effects();
        void load_bounding_boxes();
        
        void draw_triangles_mesh(const SPODMesh &mesh, int index);
        void draw_strip_mesh(const SPODMesh &mesh, int index);
        void draw_mesh(const SPODMesh &mesh, int index);
    public:
        PVRTBOUNDINGBOX model_box;
        
        mre::light light;
        mre::camera camera;
        
        model(std::string dir, std::string pod);
        ~model();
        
        void load_effects(CPVRTPFXParser &parser);
        
        const PVRTMat4& get_world() const;
        
        void render();
        
        void set_light_index(int index);
        void set_selected_node(int index);
        
        int get_select_node() const;
        
        std::string get_node_name(int index) const;
        int get_node_count() const;
        int get_node_index(const std::string& name) const;
        
        int get_node_at_pos(int x, int y, int width, int height);
        
        void set_node_overrides(int node, const effect_overrides& o);
    };
}

#endif
