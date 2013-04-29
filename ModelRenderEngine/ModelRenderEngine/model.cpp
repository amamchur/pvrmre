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
#include <sys/time.h>

#define CAM_NEAR 1
#define CAM_FAR 1000000

namespace mre {
    model::model(std::string dir, std::string pod)
    : pod_directory(dir)
    , distance(-1)
    , dirty_view_projection(true)
    , aspect(1)
    , right_rotation(0.0)
    , up_rotation(0.0)
    , viewport_translation(0.0)
    , selected_node_index(-1) {
        std::string file = dir + "/" + pod;
        pod_model = new CPVRTModelPOD();
        EPVRTError error = pod_model->ReadFromFile(file.c_str());
        if (error == PVR_SUCCESS) {
            load_bounding_boxes();
            load_vbo();
            load_nodes();
            load_effects();
            setup_default_camera();
        }
    }
    
    model::~model() {
        const PVRTuint32 count = pod_model->nNumMesh;
        glDeleteBuffers(count, vbos);
        glDeleteBuffers(count, indexVbo);
        
        GLsizei n = textures.size();
        GLuint *tmp = new GLuint[n];
        int pos = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter) {
            tmp[pos++] = iter->second;
        }
        glGenTextures(n, tmp);
        
        for (auto iter = effects.begin(); iter != effects.end(); ++iter) {
            delete iter->second;
        }
        
        delete tmp;        
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
    
    void model::load_nodes() {
        const PVRTuint32 count = pod_model->nNumNode;
        for (int i = 0; i < count; i++) {
            const SPODNode &node = pod_model->pNode[i];
            mre::node n(node.pszName, i);
            nodes.push_back(n);
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
            effect->Load(parser, iter->first.c_str(), file.c_str(), this, unknownUniforms, &errorStr);
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
        delete boxes;
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
    
    double model::get_distance() const {
        return distance;
    }
    
    void model::set_distance(double d) {
        distance = d;
        dirty_view_projection = true;
    }
    
    double model::get_up_rotation() const {
        return up_rotation;
    }
    
    void model::set_up_rotation(double r) {
        up_rotation = r;
        dirty_view_projection = true;
    }
    
    double model::get_right_rotation() const {
        return right_rotation;
    }
    
    void model::set_right_rotation(double r) {
        right_rotation = r;
        dirty_view_projection = true;
    }
    
    const PVRTVec3& model::get_viewport_translation() const {
        return viewport_translation;
    }
    
    void model::set_viewport_translation(const PVRTVec3& t) {
        viewport_translation = t;
        dirty_view_projection = true;
    }
    
    void model::recalc_view_projection() {
        if (!dirty_view_projection) {
            return;
        }
        
        PVRTVec3 cam_pos(target_pos);
        cam_pos.z += distance;
        
        PVRTVec3 forward = cam_pos - target_pos;
        PVRTVec3 up(0.0, 1.0, 0.0);
        PVRTVec3 right(1.0, 0.0, 0.0);
        up.normalize();
        right.normalize();
        
        PVRTQUATERNION qu, qr, q;
        PVRTMatrixQuaternionRotationAxis(qu, up, up_rotation);
        PVRTMatrixQuaternionRotationAxis(qr, right, right_rotation);
        PVRTMatrixQuaternionMultiply(q, qu, qr);
        
        PVRTMat4 m;
        PVRTMatrixRotationQuaternion(m, q);
        
        eye_pos = forward * m + target_pos;
        up = up * m;
        
        PVRTMat4 vpt = PVRTMat4::Translation(viewport_translation);        
        view = vpt * PVRTMat4::LookAtRH(eye_pos, target_pos, up);
        projection = PVRTMat4::PerspectiveFovRH(M_PI_4, aspect, CAM_NEAR, CAM_FAR, PVRTMat4::OGL, false);
        world = PVRTMat4::Identity();
        
        dirty_view_projection = false;
    }
    
    void model::setup_default_camera() {
        light_pos = pod_model->GetLightPosition(0);
        light_dir = pod_model->GetLightDirection(0);
        
        up_rotation = 0;
        right_rotation = 0;
        viewport_translation *= 0;
        
        distance = std::max((double)model_box.Point[7].x, (double)model_box.Point[7].y);
        distance = std::max((double)distance, (double)model_box.Point[7].z);
        distance *= 2;
        
        target_pos.x = (model_box.Point[0].x + model_box.Point[7].x) / 2;
        target_pos.y = (model_box.Point[0].y + model_box.Point[7].y) / 2;
        target_pos.z = (model_box.Point[0].z + model_box.Point[7].z) / 2;
        
        dirty_view_projection = true;
    }
    
    void model::setup(float aspect) {
        if (this->aspect == aspect) {
            return;
        }
        
        this->aspect = aspect;
        dirty_view_projection = true;
        recalc_view_projection();
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
    
    void model::render() {
        recalc_view_projection();
                               
        glEnable(GL_CULL_FACE);
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
        if (index < 0 || index >= pod_model->nNumMeshNode) {
            return "";
        }
        
        const SPODNode &node = pod_model->pNode[index];
        return node.pszName;
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
    
    GLuint model::get_texture(const std::string& name) {
        auto pos = textures.find(name);        
        if (pos != textures.end()) {
            return pos->second;
        }
        
        std::string path = pod_directory + "/" + name;
        GLuint uiHandle = 0;
        if(PVRTTextureLoadFromPVR(path.c_str(), &uiHandle) != PVR_SUCCESS) {
            return 0;
        }
        
        textures[name] = uiHandle;
        return uiHandle;
    }
    
    EPVRTError model::PVRTPFXOnLoadTexture(const CPVRTStringHash& TextureName, GLuint& uiHandle, unsigned int& uiFlags) {
        std::string path = pod_directory + "/" + TextureName.c_str();
        if(PVRTTextureLoadFromPVR(path.c_str(), &uiHandle) != PVR_SUCCESS) {
            return PVR_FAIL;
        }
        
        textures[TextureName.c_str()] = uiHandle;
        return PVR_SUCCESS;
    }
}
