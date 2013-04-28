//
//  Model.h
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

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
        
        CPVRTModelPOD *podModel;
        std::string podDir;        
        
        GLuint *vbos;
        GLuint *indexVbo;
        
        PVRTMat4 projection;
        PVRTMat4 view;
        PVRTMat4 world;
        PVRTVec3 eye_pos;
        PVRTVec3 light_pos;
        PVRTVec3 light_dir;
        
        nodes_list nodes;
        effects_map effects;
        textures_map textures;
        nodes_overrides_map nodes_overrides;
        
        int selected_node_index;
        
        void load_vbo();
        void load_nodes();
        void load_effects();
        
        void draw_triangles_mesh(const SPODMesh &mesh, int index);
        void draw_strip_mesh(const SPODMesh &mesh, int index);
        void draw_mesh(const SPODMesh &mesh, int index);
        
        void configure_effect();
        void cleanup_effect();
        
        EPVRTError PVRTPFXOnLoadTexture(const CPVRTStringHash& TextureName, GLuint& uiHandle, unsigned int& uiFlags);
    public:
        model(std::string dir, std::string pod);
        ~model();
        
        const PVRTMat4& get_projection() const;
        const PVRTMat4& get_view() const;
        const PVRTMat4& get_world() const;
        const PVRTVec3& get_eye_pos() const;
        const PVRTVec3& get_light_pos() const;
        const PVRTVec3& get_light_dir() const;
        
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
