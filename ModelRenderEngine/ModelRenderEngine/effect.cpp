//
//  effect.cpp
//  ModelRenderEngine
//
//  Created by admin on 4/28/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#include "effect.h"

namespace mre {
    effect::effect(mre::model *model, CPVRTPFXEffect *pfx_effect)
        : model(model)
        , pfx_effect(pfx_effect) {        
    }
    
    void effect::configure(const SPODMesh &mesh, const SPODMaterial &material) {
        pfx_effect->Activate();
        
        PVRTMat4 projection = model->get_projection();
        PVRTMat4 view = model->get_view();
        PVRTMat4 world = model->get_world();        
        PVRTMat4 world_view = view * world;
        PVRTMat4 mvp = projection * world_view;
        PVRTMat4 world_view_i = world_view.inverse();
        PVRTMat4 world_view_it = world_view_i.transpose();
        PVRTVec3 eye_pos = model->get_eye_pos();
        PVRTVec3 light_pos = model->get_light_pos();
        PVRTVec3 light_dir = model->get_light_dir();        
        
        const CPVRTArray<SPVRTPFXUniform>& uniforms = pfx_effect->GetUniformArray();        
        for(int i = 0; i < uniforms.GetSize(); ++i) {
            GLuint location = uniforms[i].nLocation;
            EPVRTPFXUniformSemantic semantic = (EPVRTPFXUniformSemantic)uniforms[i].nSemantic;
            switch (semantic) {
                case ePVRTPFX_UsPOSITION:
                    glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, mesh.sVertex.nStride, mesh.sVertex.pData);
                    break;
                case ePVRTPFX_UsNORMAL:
                    glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, mesh.sNormals.nStride, mesh.sNormals.pData);
                    break;
                case ePVRTPFX_UsUV:
                    glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, mesh.psUVW[0].nStride, mesh.psUVW[0].pData);
                    break;
                case ePVRTPFX_UsWORLDVIEWPROJECTION:
					glUniformMatrix4fv(location, 1, GL_FALSE, mvp.ptr());
                    break;
                case ePVRTPFX_UsWORLDVIEWI:
					glUniformMatrix3fv(location, 1, GL_FALSE, PVRTMat3(world_view_i).ptr());
                    break;
                case ePVRTPFX_UsWORLD:
                    glUniformMatrix4fv(location, 1, GL_FALSE, world.ptr());
                    break;
                case ePVRTPFX_UsWORLDVIEWIT:
					glUniformMatrix3fv(location, 1, GL_FALSE, PVRTMat3(world_view_it).ptr());
                    break;
                case ePVRTPFX_UsLIGHTPOSMODEL:
                    glUniform3fv(location, 1, light_pos.ptr());
                    break;
                case ePVRTPFX_UsLIGHTPOSWORLD:
                    glUniform3fv(location, 1, light_pos.ptr());
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
                case ePVRTPFX_UsTEXTURE:
					glUniform1i(location, uniforms[i].nIdx);
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
                default:
                    break;
            }
        }
    }
    
    void effect::cleanup() {
        
    }
}