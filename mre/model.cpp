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

#include "model.h"
#include "effect.h"
#include "texture_buffer.h"

#include <map>
#include <sys/time.h>

#define CAM_NEAR 1
#define CAM_FAR 10000

namespace mre {
    model::model(std::string dir, std::string pod)
    : pod_directory(dir)
    , selected_node_index(-1) {
        std::string file = dir + "/" + pod;
        pod_model = new CPVRTModelPOD();
        EPVRTError error = pod_model->ReadFromFile(file.c_str());
        if (error == PVR_SUCCESS) {
            load_bounding_boxes();
            load_vbo();
            load_effects();
        }
    }
    
    model::~model() {
        const PVRTuint32 count = pod_model->nNumMesh;
        glDeleteBuffers(count, vbos);
        glDeleteBuffers(count, indexVbo);
        
        for (auto iter = effects.begin(); iter != effects.end(); ++iter) {
            delete iter->second;
        }
        
        delete vbos;
        delete indexVbo;
        delete pod_model;
    }
    
    void model::load_vbo() {
        const PVRTuint32 count = pod_model->nNumMesh;
        vbos = new GLuint[count];
        indexVbo = new GLuint[count];
        
        glGenBuffers(count, vbos);
        for(int i = 0; i < count; i++) {
            const SPODMesh& mesh = pod_model->pMesh[i];
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
    
    void model::load_effects(CPVRTPFXParser &parser) {
        unsigned int count = parser.GetNumberEffects();
        unsigned int unknownUniforms = 0;
        CPVRTString	errorStr;
        for (unsigned int i = 0; i < count; i++) {
            SPVRTPFXParserEffect pe = parser.GetEffect(i);
            CPVRTPFXEffect *effect = new CPVRTPFXEffect();
            effect->Load(parser, pe.Name.c_str(), parser.GetPFXFileName().c_str(), NULL, unknownUniforms, &errorStr);
            effects[pe.Name.c_str()] = effect;
        }
    }
    
    void model::load_effects() {
        const CPVRTModelPOD &pod = *pod_model;
        std::map<std::string, int> effectsFile;
        std::map<std::string, std::string> effectsMap;
        for (int i = 0; i < pod.nNumMaterial; ++i) {
            const SPODMaterial &material = pod.pMaterial[i];
            effectsFile[material.pszEffectFile] = 1;
            effectsMap[material.pszEffectName] = material.pszEffectFile;
        }
        
        std::string file = pod_directory + "/" + effectsFile.begin()->first;
        CPVRTPFXParser parser;
        CPVRTString	errorStr;
        parser.ParseFromFile(file.c_str(), &errorStr);
        
        unsigned int unknownUniforms = 0;
        for (auto iter = effectsMap.begin(); iter != effectsMap.end(); ++iter) {
            CPVRTPFXEffect *effect = new CPVRTPFXEffect();
            effect->Load(parser, iter->first.c_str(), file.c_str(), NULL, unknownUniforms, &errorStr);
            effects[iter->first] = effect;
        }
    }
    
    void model::load_bounding_boxes() {
        pod_model->SetFrame(0);
        int count = pod_model->nNumMeshNode;
        PVRTBOUNDINGBOX *boxes = new PVRTBOUNDINGBOX[count];
        for (int i = 0; i < count; i++) {
            const SPODNode &node = pod_model->pNode[i];
            const SPODMesh &mesh = pod_model->pMesh[node.nIdx];
            PVRTMat4 world = pod_model->GetWorldMatrix(node);
            PVRTBOUNDINGBOX box;
            PVRTBoundingBoxComputeInterleaved(&box, mesh.pInterleaved, mesh.nNumVertex, 0, mesh.sVertex.nStride);
            PVRTTransformArray(boxes[i].Point, box.Point, sizeof(box.Point)/sizeof(*box.Point), &world);
        }
        PVRTBoundingBoxCompute(&model_box, boxes->Point, count * 8);
        delete[] boxes;
    }
    
    const PVRTMat4& model::get_world() const {
        return world;
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
                GLvoid* indices = reinterpret_cast<GLvoid*>(offset * mesh.sFaces.nStride);
                glDrawElements(GL_TRIANGLE_STRIP, mesh.pnStripLength[i]+2, type, indices);
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
    
    void model::render() {                               
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        for (int i = 0; i < pod_model->nNumMeshNode; i++) {
            const SPODNode &node = pod_model->pNode[i];
            const SPODMaterial &material = pod_model->pMaterial[node.nIdxMaterial];
            const SPODMesh &mesh = pod_model->pMesh[node.nIdx];
            world = pod_model->GetWorldMatrix(node);
            
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
    
    void model::set_light_index(int index) {
        light.position = pod_model->GetLightPosition(index);
        light.direction = pod_model->GetLightDirection(index);
        light.color = pod_model->pLight[index].pfColour;
    }
    
    void model::set_selected_node(int index) {
        selected_node_index = index;
    }
    
    int model::get_select_node() const {
        return selected_node_index;
    }
    
    int model::get_node_at_pos(int x, int y, int width, int height) {        
        texture_buffer text_buffer(width, height, TRUE); // Render to texture and auto delete

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                       
        CPVRTPFXEffect *pfx_effect = effects["Selection"];
        mre::effect effect(*this, pfx_effect);
        
        for (int i = 0; i < pod_model->nNumMeshNode; i++) {
            unsigned int color = ((i * 13 + 17) << 8);
            GLubyte *colors = reinterpret_cast<GLubyte *>(&color);
            PVRTVec3 vec(colors[0] / 255.0f, colors[1] / 255.0f, colors[2] / 255.0f);
            effect.uniform_overrides.clear();
            effect.uniform_overrides[ePVRTPFX_UsMATERIALCOLORDIFFUSE] = vec;
            
            const SPODNode &node = pod_model->pNode[i];
            const SPODMaterial &material = pod_model->pMaterial[node.nIdxMaterial];
            const SPODMesh &mesh = pod_model->pMesh[node.nIdx];
            world = pod_model->GetWorldMatrix(node);
            
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo[i]);
                        
            effect.configure(mesh, material);
            draw_mesh(mesh, i);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            effect.cleanup();
        }
                
        GLubyte buffer[64] = {0};
        glReadPixels(x, height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        
        int res = -1;
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
        if (index < 0 || index >= pod_model->nNumMeshNode) {
            return "";
        }
        
        const SPODNode &node = pod_model->pNode[index];
        return node.pszName;
    }
    
    int model::get_node_count() const {
        return pod_model->nNumMeshNode;
    }
    
    int model::get_node_index(const std::string& name) const {
        for (int i = 0; i < pod_model->nNumMeshNode; i++) {
            const SPODNode &node = pod_model->pNode[i];
            if (name == node.pszName) {
                return i;
            }
        }
        return -1;
    }
}
