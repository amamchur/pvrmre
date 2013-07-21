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

#ifndef PVRMRE_EFFECT_H
#define PVRMRE_EFFECT_H

#include <map>

#include "OGLES2Tools.h"
#include "model.h"

namespace mre {
    class uniform_override;
    class texture_override;
    
    typedef std::map<int, uniform_override> uniform_overrides_map;
    typedef std::map<int, texture_override> texture_overrides_map;
    
    class uniform_override {
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
        
        uniform_override();
        uniform_override(const uniform_override& uo);
        uniform_override(const PVRTMat4& mat4);
        uniform_override(const PVRTMat3& mat3);
        uniform_override(const PVRTVec4& vec4);
        uniform_override(const PVRTVec3& vec3);
        
        operator const GLfloat*() const;
        operator GLint() const;
        
        uniform_override& operator=(const uniform_override& uo);
    };
    
    class texture_override {
    public:
        GLuint unit;
        GLuint textId;
    };
    
    class effect_overrides {
    public:
        uniform_overrides_map uniform_overrides;
        texture_overrides_map texture_overrides;
    };
    
    class effect {
    private:
        CPVRTPFXEffect *pfx_effect;
        const mre::model &model;
        
        void apply_override(const uniform_override& override, GLuint location);
        void apply_default(EPVRTPFXUniformSemantic semantic, GLuint location, const SPODMaterial &material);
    public:
        uniform_overrides_map uniform_overrides;
        texture_overrides_map texture_overrides;
        
        effect(const mre::model &model, CPVRTPFXEffect *pfx_effect);
        
        void configure(const SPODMesh &mesh, const SPODMaterial &material);
        void cleanup();
    };
}

#endif