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
#include "OGLES2Tools.h"

#include "node.h"

namespace mre {
    class model {
    private:
        CPVRTModelPOD *podModel;
        
        GLuint *vbos;
        GLuint *indexVbo;
        
        PVRTMat4 projection;
        PVRTMat4 view;
        PVRTMat4 world;
        
        std::list<mre::node> nodes;
        
        void load_vbo();
        void load_nodes();
    public:
        model(CPVRTModelPOD *pod);
        ~model();
        
        void render();
    };
}

#endif
