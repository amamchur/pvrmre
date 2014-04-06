//
//  Copyright 2013, Andrii Mamchur
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

namespace mre {
    class node;
    class effect_overrides;
    
    class model : public PVRTPFXEffectDelegate {
    private:
        typedef std::map<std::string, CPVRTPFXEffect*> effects_map;
        typedef std::map<std::string, GLuint> textures_map;
        typedef std::map<int, effect_overrides> nodes_overrides_map;
        typedef std::list<mre::node> nodes_list;
        
        CPVRTModelPOD *pod_model;
        std::string pod_directory;
        
        GLuint *vbos;
        GLuint *indexVbo;
        
        double distance;
        double aspect;
        double up_rotation;
        double right_rotation;
        bool dirty_view_projection;
        
        PVRTMat4 projection;
        PVRTMat4 view;
        PVRTMat4 world;
        PVRTVec3 target_pos;
        PVRTVec3 eye_pos;
        PVRTVec3 light_pos;
        PVRTVec3 light_dir;
        PVRTVec3 viewport_translation;
        
        nodes_list nodes;
        effects_map effects;
        textures_map textures;
        nodes_overrides_map nodes_overrides;
        
        PVRTBOUNDINGBOX model_box;

        int selected_node_index;
        
        void load_vbo();
        void load_nodes();
        void load_effects();
        void load_bounding_boxes();
        
        void draw_triangles_mesh(const SPODMesh &mesh, int index);
        void draw_strip_mesh(const SPODMesh &mesh, int index);
        void draw_mesh(const SPODMesh &mesh, int index);
        
        EPVRTError PVRTPFXOnLoadTexture(const CPVRTStringHash& TextureName, GLuint& uiHandle, unsigned int& uiFlags);
    public:
        
        model(std::string dir, std::string pod);
        ~model();
        
        void load_effects(CPVRTPFXParser &parser);
        
        const PVRTMat4& get_projection() const;
        const PVRTMat4& get_view() const;
        const PVRTMat4& get_world() const;
        const PVRTVec3& get_eye_pos() const;
        const PVRTVec3& get_light_pos() const;
        const PVRTVec3& get_light_dir() const;
        
        const PVRTVec3& get_viewport_translation() const;
        double get_distance() const;
        double get_up_rotation() const;
        double get_right_rotation() const;
        
        void set_viewport_translation(const PVRTVec3& t);
        void set_distance(double d);
        void set_up_rotation(double r);
        void set_right_rotation(double r);

        void recalc_view_projection();
        void setup_default_camera();
        void setup(float aspect);
        void render();
        
        void set_selected_node(int index);
        int get_select_node() const;
        
        std::string get_node_name(int index) const;
        int get_node_index(const std::string& name) const;
        
        int get_node_at_pos(int x, int y, int width, int height);
        
        void set_node_overrides(int node, const effect_overrides& o);
        
        GLuint get_texture(const std::string& name);
    };
}

#endif
