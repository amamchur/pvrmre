//
//  Model.cpp
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#include "model.h"
#include "effect.h"
#include "node.h"

#include <map>

#define CAM_NEAR 1
#define CAM_FAR 1000000

namespace mre {
    model::model(std::string dir, std::string pod)
    : podDir(dir)
    , selected_node_index(-1) {
        std::string file = dir + "/" + pod;
        podModel = new CPVRTModelPOD();
        EPVRTError error = podModel->ReadFromFile(file.c_str());
        if (error == PVR_SUCCESS) {
            load_vbo();
            load_nodes();
            load_effects();
        }
    }
    
    model::~model() {
        const PVRTuint32 count = podModel->nNumMesh;
        glDeleteBuffers(count, vbos);
        glDeleteBuffers(count, indexVbo);
        
        GLsizei n = textures.size();
        GLuint *tmp = new GLuint[n];
        int pos = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter) {
            tmp[pos++] = iter->second;
        }
        glGenTextures(n, tmp);
        
        delete tmp;        
        delete vbos;
        delete indexVbo;
        delete podModel;
    }
    
    void model::load_vbo() {
        const PVRTuint32 count = podModel->nNumMesh;
        vbos = new GLuint[count];
        indexVbo = new GLuint[count];
        
        glGenBuffers(count, vbos);
        for(int i = 0; i < count; i++) {
            const SPODMesh& mesh = podModel->pMesh[i];
            PVRTuint32 size = mesh.nNumVertex * mesh.sVertex.nStride;
            
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
            glBufferData(GL_ARRAY_BUFFER, size, mesh.pInterleaved, GL_STATIC_DRAW);
            
            indexVbo[i] = 0;
            if (mesh.sFaces.pData) {
                glGenBuffers(1, indexVbo + i);
                size = PVRTModelPODCountIndices(mesh) * mesh.sFaces.nStride;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo[i]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, mesh.sFaces.pData, GL_STATIC_DRAW);
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    void model::load_nodes() {
        const PVRTuint32 count = podModel->nNumNode;
        for (int i = 0; i < count; i++) {
            const SPODNode &node = podModel->pNode[i];
            mre::node n(node.pszName, i);
            nodes.push_back(n);
        }
    }
    
    void model::load_effects() {
        const CPVRTModelPOD &pod = *podModel;
        std::map<std::string, int> effectsFile;
        std::map<std::string, std::string> effectsMap;
        for (int i = 0; i < pod.nNumMaterial; ++i) {
            const SPODMaterial &material = pod.pMaterial[i];
            effectsFile[material.pszEffectFile] = 1;
            effectsMap[material.pszEffectName] = material.pszEffectFile;
        }
        
        std::string file = podDir + "/" + effectsFile.begin()->first;
        CPVRTPFXParser parser;
        CPVRTString	errorStr;
        EPVRTError error = parser.ParseFromFile(file.c_str(), &errorStr);
        
        unsigned int unknownUniforms = 0;
        for (auto iter = effectsMap.begin(); iter != effectsMap.end(); ++iter) {
            CPVRTPFXEffect *effect = new CPVRTPFXEffect();
            error = effect->Load(parser, iter->first.c_str(), file.c_str(), this, unknownUniforms, &errorStr);
            if (error != PVR_SUCCESS) {
                delete effect;
            }
            
            effects[iter->first] = effect;
        }
    }
    
    const PVRTMat4& model::get_projection() const {
        return projection;        
    }
    
    const PVRTMat4& model::get_view() const {
        return view;
    }
    
    const PVRTMat4& model::get_world() const {
        return world;
    }
    
    const PVRTVec3& model::get_eye_pos() const {
        return eye_pos;
    }
    
    const PVRTVec3& model::get_light_pos() const {
        return light_pos;
    }
    
    const PVRTVec3& model::get_light_dir() const {
        return light_dir;
    }
    
    void model::setup(float aspect) {
        podModel->SetFrame(0);
        
        light_pos = podModel->GetLightPosition(1);
        light_dir = podModel->GetLightDirection(1);
        
        PVRTVec3 cameraFrom, cameraTo, cameraUp;
        VERTTYPE cameraFOV = podModel->GetCamera(cameraFrom, cameraTo, cameraUp, 0);
        
        view = PVRTMat4::LookAtRH(cameraFrom, cameraTo, cameraUp);
        eye_pos = cameraFrom;
        projection = PVRTMat4::PerspectiveFovRH(cameraFOV, aspect, CAM_NEAR, CAM_FAR, PVRTMat4::OGL, false);
        world = PVRTMat4::Identity();
    }
    
    void model::draw_triangles_mesh(const SPODMesh &mesh, int index) {
        if(indexVbo[index]) {
            GLenum type = (mesh.sFaces.eType == EPODDataUnsignedShort) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
            glDrawElements(GL_TRIANGLES, mesh.nNumFaces * 3, type, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.nNumFaces * 3);
        }
    }
    
    void model::draw_strip_mesh(const SPODMesh &mesh, int index) {
        PVRTuint32 offset = 0;        
        GLenum type = (mesh.sFaces.eType == EPODDataUnsignedShort) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;        
        for(int i = 0; i < (int)mesh.nNumStrips; ++i) {
            if (indexVbo[index]) {
                glDrawElements(GL_TRIANGLE_STRIP, mesh.pnStripLength[i]+2, type, (void*)(offset * mesh.sFaces.nStride));
            } else {
                glDrawArrays(GL_TRIANGLE_STRIP, offset, mesh.pnStripLength[i]+2);
            }
            offset += mesh.pnStripLength[i] + 2;
        }
    }
    
    void model::draw_mesh(const SPODMesh &mesh, int index) {
        if (mesh.nNumStrips == 0) {
            draw_triangles_mesh(mesh, index);
        } else {
            draw_strip_mesh(mesh, index);
        }
    }
    
    void model::configure_effect() {
        
    }
    
    void model::cleanup_effect() {
        
    }
    
    void model::render() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        for (int i = 0; i < podModel->nNumMeshNode; i++) {
            const SPODNode &node = podModel->pNode[i];
            const SPODMaterial &material = podModel->pMaterial[node.nIdxMaterial];
            const SPODMesh &mesh = podModel->pMesh[node.nIdx];
            world = podModel->GetWorldMatrix(node);
            
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo[i]);
            
            CPVRTPFXEffect *pfx_effect = NULL;
            if (i == selected_node_index) {
                pfx_effect = effects["SelectedNode"];
            } else {
                pfx_effect = effects[material.pszEffectName];
            }
            mre::effect effect(*this, pfx_effect);
            auto pos = nodes_overrides.find(i);
            if (pos != nodes_overrides.end()) {
                const effect_overrides& eo = pos->second;
                effect.uniform_overrides = eo.uniform_overrides;
                effect.texture_overrides = eo.texture_overrides;
            }
            
            effect.configure(mesh, material);
            draw_mesh(mesh, i);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);            
            effect.cleanup();
        }
    }
    
    void model::set_selected_node(int index) {
        selected_node_index = index;
    }
    
    int model::get_select_node() const {
        return selected_node_index;
    }
    
    int model::get_node_at_pos(int x, int y, int width, int height) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        int res = 0;
               
        CPVRTPFXEffect *pfx_effect = effects["Selection"];
        mre::effect effect(*this, pfx_effect);
        
        glDisable(GL_DITHER);
        for (int i = 0; i < podModel->nNumMeshNode; i++) {
            unsigned int color = ((i * 13 + 17) << 8);
            GLubyte *colors = reinterpret_cast<GLubyte *>(&color);
            PVRTVec3 vec(colors[0] / 255.0f, colors[1] / 255.0f, colors[2] / 255.0f);
            effect.uniform_overrides.clear();
            effect.uniform_overrides[ePVRTPFX_UsMATERIALCOLORDIFFUSE] = vec;
            
            const SPODNode &node = podModel->pNode[i];
            const SPODMaterial &material = podModel->pMaterial[node.nIdxMaterial];
            const SPODMesh &mesh = podModel->pMesh[node.nIdx];
            world = podModel->GetWorldMatrix(node);
            
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo[i]);
                        
            effect.configure(mesh, material);
            draw_mesh(mesh, i);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            effect.cleanup();
        }
        
        glEnable(GL_DITHER);
                
        GLubyte buffer[64] = {0};
        glReadPixels(x, height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        
        GLenum error = glGetError();
        if (error == GL_NO_ERROR) {
            unsigned int color = *reinterpret_cast<unsigned int *>(buffer);
            if (color == 0xFFFFFFFF) {
                res = -1;
            } else {
                res = (((color & 0xFFFFFF) >> 8) - 17) / 13;
            }
        }
        return res;
    }
    
    void model::set_node_overrides(int node, const effect_overrides& o) {
        nodes_overrides[node] = o;
    }
    
    std::string model::get_node_name(int index) const {
        if (index < 0 || index >= podModel->nNumMeshNode) {
            return "";
        }
        
        const SPODNode &node = podModel->pNode[index];
        return node.pszName;
    }
    
    int model::get_node_index(const std::string& name) const {
        for (int i = 0; i < podModel->nNumMeshNode; i++) {
            const SPODNode &node = podModel->pNode[i];
            if (name == node.pszName) {
                return i;
            }
        }
        return -1;
    }
    
    GLuint model::get_texture(const std::string& name) {
        auto pos = textures.find(name);        
        if (pos != textures.end()) {
            return pos->second;
        }
        
        std::string path = podDir + "/" + name;
        GLuint uiHandle = 0;
        if(PVRTTextureLoadFromPVR(path.c_str(), &uiHandle) != PVR_SUCCESS) {
            return 0;
        }
        
        textures[name] = uiHandle;
        return uiHandle;
    }
    
    EPVRTError model::PVRTPFXOnLoadTexture(const CPVRTStringHash& TextureName, GLuint& uiHandle, unsigned int& uiFlags) {
        std::string path = podDir + "/" + TextureName.c_str();
        if(PVRTTextureLoadFromPVR(path.c_str(), &uiHandle) != PVR_SUCCESS) {
            return PVR_FAIL;
        }
        
        textures[TextureName.c_str()] = uiHandle;
        return PVR_SUCCESS;
    }
}
