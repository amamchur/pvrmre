//
//  Model.cpp
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#include "model.h"

namespace mre {
    model::model(CPVRTModelPOD *pod)
        : podModel(pod)
    {
        load_vbo();
        load_nodes();
    }
    
    model::~model() {
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
    
    void model::render() {
    }
}