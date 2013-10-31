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

#include "effect.h"
#include <sys/time.h>

namespace mre {
    uniform_override::uniform_override() {
        type = overide_type_none;
        memset(buffer, 0, sizeof(buffer));
    }
    
    uniform_override::uniform_override(const uniform_override& uo)
    {
        type = uo.type;
        memcpy(buffer, uo.buffer, sizeof(buffer));
    }
    
    uniform_override::uniform_override(const PVRTMat4& mat4) {
        type = overide_type_mat4;
        memcpy(buffer, mat4.f, sizeof(mat4.f));
    }
    
    uniform_override::uniform_override(const PVRTMat3& mat3) {
        type = overide_type_mat3;
        memcpy(buffer, mat3.f, sizeof(mat3.f));
    }
    
    uniform_override::uniform_override(const PVRTVec4& vec4) {
        type = overide_type_vec4;
        memcpy(buffer, &vec4.x, sizeof(const PVRTVECTOR4));
    }
    
    uniform_override::uniform_override(const PVRTVec3& vec3) {
        type = overide_type_vec3;
        memcpy(buffer, &vec3.x, sizeof(PVRTVECTOR3));
    }
    
    uniform_override& uniform_override::operator=(const uniform_override& uo) {
        type = uo.type;
        memcpy(buffer, uo.buffer, sizeof(buffer));
        return *this;
    }
    
    const void* uniform_override::ptr() const {
        return buffer;
    }
    
    uniform_override::uniform_overide_type uniform_override::get_type() const {
        return type;
    }
    
    uniform_override::operator const GLfloat*() const {
        return reinterpret_cast<const GLfloat*>(buffer);
    }
    
    uniform_override::operator GLint() const {
        return *reinterpret_cast<const GLint*>(buffer);
    }
    
    effect::effect(const mre::model &model, CPVRTPFXEffect *pfx_effect)
        : model(model)
        , pfx_effect(pfx_effect) {        
    }
    
    void effect::apply_override(const uniform_override& override, GLuint location) {
        auto type = override.get_type();
        switch (type) {
            case uniform_override::overide_type_mat4:
                glUniformMatrix4fv(location, 1, GL_FALSE, override);
                break;
            case uniform_override::overide_type_mat3:
                glUniformMatrix3fv(location, 1, GL_FALSE, override);
                break;
            case uniform_override::overide_type_vec4:
                glUniform4fv(location, 1, override);
                break;
            case uniform_override::overide_type_vec3:
                glUniform3fv(location, 1, override);
                break;
            default:
                break;
        }
    }
    
    void effect::apply_default(EPVRTPFXUniformSemantic semantic, GLuint location, const SPODMaterial &material) {
        PVRTMat4 projection = model.get_projection();
        PVRTMat4 view = model.get_view();
        PVRTMat4 world = model.get_world();
        PVRTMat4 world_view = view * world;
        PVRTMat4 mvp = projection * world_view;
        PVRTMat4 world_view_i = world_view.inverse();
        PVRTMat4 world_view_it = world_view_i.transpose();
        PVRTVec3 eye_pos = model.get_eye_pos();
        PVRTVec3 light_pos = model.get_light_pos();
        PVRTVec3 light_dir = model.get_light_dir();
        PVRTVec3 light_pos_eye = light_pos * world_view_i;
        PVRTVec3 light_pos_model = light_pos * world;
        
        timeval time;
        gettimeofday(&time, NULL);
        double sec = time.tv_sec + ((double)time.tv_usec / 1000000.0);
        
        switch (semantic) {
            case ePVRTPFX_UsWORLDVIEWPROJECTION:
                glUniformMatrix4fv(location, 1, GL_FALSE, mvp.ptr());
                break;
            case ePVRTPFX_UsWORLDVIEWI:
                glUniformMatrix3fv(location, 1, GL_FALSE, PVRTMat3(world_view_i).ptr());
                break;
            case ePVRTPFX_UsWORLD:
                glUniformMatrix4fv(location, 1, GL_FALSE, world.ptr());
                break;
            case ePVRTPFX_UsWORLDVIEW:
                glUniformMatrix4fv(location, 1, GL_FALSE, world_view.ptr());
                break;
            case ePVRTPFX_UsWORLDVIEWIT:
                glUniformMatrix3fv(location, 1, GL_FALSE, PVRTMat3(world_view_it).ptr());
                break;
            case ePVRTPFX_UsLIGHTPOSMODEL:
                glUniform3fv(location, 1, light_pos.ptr());
                break;
            case ePVRTPFX_UsLIGHTPOSEYE:
                glUniform3fv(location, 1, light_pos_eye.ptr());
                break;
            case ePVRTPFX_UsLIGHTPOSWORLD:
                glUniform3fv(location, 1, light_pos_model.ptr());
                break;
            case ePVRTPFX_UsLIGHTDIREYE: {
                PVRTVec4 ld = PVRTVec4(light_dir, 0);
                ld.x = -ld.x;
                ld.y = -ld.y;
                ld.z = -ld.z;
                ld.w = 0;
                
                PVRTVec4 space = view * ld;
                glUniform3f(location, space.x, space.y, space.z);
                break;
            }
            case ePVRTPFX_UsLIGHTDIRWORLD:
                glUniform3fv(location, 1, light_dir.ptr());
                break;
            case ePVRTPFX_UsEYEPOSWORLD:
                glUniform3fv(location, 1, eye_pos.ptr());
                break;
            case ePVRTPFX_UsMATERIALCOLORAMBIENT:
                glUniform3fv(location, 1, material.pfMatAmbient);
                break;
            case ePVRTPFX_UsMATERIALCOLORDIFFUSE:
                glUniform3fv(location, 1, material.pfMatDiffuse);
                break;
            case ePVRTPFX_UsMATERIALCOLORSPECULAR:
                glUniform3fv(location, 1, material.pfMatSpecular);
                break;
            case ePVRTPFX_UsMATERIALSHININESS:
                glUniform1f(location, material.fMatShininess);
                break;
            case ePVRTPFX_UsTIME:
                glUniform1f(location, sec);
                break;
            case ePVRTPFX_UsTIMECOS:
                glUniform1f(location, cos(sec));
                break;
            case ePVRTPFX_UsTIMESIN:
                glUniform1f(location, sin(sec));
                break;
            case ePVRTPFX_UsTIME2PI:
                glUniform1f(location, 2 * M_PI * sec);
                break;
            case ePVRTPFX_UsTIME2PICOS:
                glUniform1f(location, cos(2 * M_PI * sec));
                break;
            case ePVRTPFX_UsTIME2PISIN:
                glUniform1f(location, sin(2 * M_PI * sec));
                break;
            default:
                break;
        }
    }
    
    void effect::configure(const SPODMesh &mesh, const SPODMaterial &material) {
        GLuint program = pfx_effect->GetProgramHandle();
        glUseProgram(program);
        
        const CPVRTArray<SPVRTPFXTexture> &textures = pfx_effect->GetTextureArray();
        for(unsigned int i = 0; i < textures.GetSize(); ++i) {
            const SPVRTPFXTexture &text = textures[i];
            glActiveTexture(GL_TEXTURE0 + text.unit);
            GLenum target = (text.flags & PVRTEX_CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
            GLuint textId = text.ui;
            auto pos = texture_overrides.find(text.unit);
            if (pos != texture_overrides.end()) {
                textId = pos->second.textId;
            }
            glBindTexture(target, textId);
        }
        
        const CPVRTArray<SPVRTPFXUniform>& uniforms = pfx_effect->GetUniformArray();        
        for(int i = 0; i < uniforms.GetSize(); ++i) {
            GLuint location = uniforms[i].nLocation;
            EPVRTPFXUniformSemantic semantic = (EPVRTPFXUniformSemantic)uniforms[i].nSemantic;
            switch (semantic) {
                case ePVRTPFX_UsPOSITION:
                    glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, mesh.sVertex.nStride, mesh.sVertex.pData);
                    continue;
                case ePVRTPFX_UsNORMAL:
                    glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, mesh.sNormals.nStride, mesh.sNormals.pData);
                    continue;
                case ePVRTPFX_UsUV:
                    glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, mesh.psUVW[0].nStride, mesh.psUVW[0].pData);
                    continue;
                case ePVRTPFX_UsTEXTURE:
                    glUniform1i(location, uniforms[i].nIdx);
                    continue;
                default:
                    break;
            }
            
            auto pos = uniform_overrides.find(semantic);
            if (pos != uniform_overrides.end()) {
                apply_override(pos->second, location);
            } else {
                apply_default(semantic, location, material);
            }            
        }
    }
    
    void effect::cleanup() {
        const CPVRTArray<SPVRTPFXUniform>& uniforms = pfx_effect->GetUniformArray();
        for(int i = 0; i < uniforms.GetSize(); ++i) {
			switch(uniforms[i].nSemantic) {
                case ePVRTPFX_UsPOSITION:
                case ePVRTPFX_UsNORMAL:
                case ePVRTPFX_UsUV:
					glDisableVertexAttribArray(uniforms[i].nLocation);
                    break;
            }
        }
        glUseProgram(0);
    }
}
