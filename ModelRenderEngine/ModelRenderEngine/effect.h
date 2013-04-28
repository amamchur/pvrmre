//
//  effect.h
//  ModelRenderEngine
//
//  Created by admin on 4/28/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#ifndef PVRMRE_EFFECT_H
#define PVRMRE_EFFECT_H

#include <map>

#include "OGLES2Tools.h"
#include "model.h"

namespace mre {
    class uniform_overide {
    public:
        typedef enum {
            overide_type_none,
            overide_type_mat4,
            overide_type_mat3,
            overide_type_vec4,
            overide_type_vec3
        } uniform_overide_type;
    private:
        float buffer[16];
        uniform_overide_type type;
    public:
        const void* ptr() const;
        uniform_overide_type get_type() const;
        
        uniform_overide();
        uniform_overide(const uniform_overide& uo);
        uniform_overide(const PVRTMat4& mat4);
        uniform_overide(const PVRTMat3& mat3);
        uniform_overide(const PVRTVec4& vec4);
        uniform_overide(const PVRTVec3& vec3);
        
        operator const GLfloat*() const;
        operator GLint() const;
        
        uniform_overide& operator=(const uniform_overide& uo);
    };
    
    class effect {
    private:
        CPVRTPFXEffect *pfx_effect;
        const mre::model &model;
        
        void apply_override(const uniform_overide& override, GLuint location);
    public:
        typedef std::map<int, uniform_overide> overides_map;
        
        overides_map overides;
        
        effect(const mre::model &model, CPVRTPFXEffect *pfx_effect);
        
        void configure(const SPODMesh &mesh, const SPODMaterial &material);
        void cleanup();
    };
}

#endif