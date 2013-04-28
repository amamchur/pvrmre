//
//  effect.h
//  ModelRenderEngine
//
//  Created by admin on 4/28/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#ifndef PVRMRE_EFFECT_H
#define PVRMRE_EFFECT_H

#include "OGLES2Tools.h"

#include "model.h"

namespace mre {
    class effect {
    private:
        CPVRTPFXEffect *pfx_effect;
        mre::model *model;
    public:
        effect(mre::model *model, CPVRTPFXEffect *pfx_effect);
        
        void configure(const SPODMesh &mesh, const SPODMaterial &material);
        void cleanup();
    };
}

#endif