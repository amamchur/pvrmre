//
//  texture_manager.cpp
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 4/8/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#include "texture_manager.h"

namespace mre {
    texture_manager::texture_manager(const std::string &dir)
    : directory(dir) {
        
    }
    
    texture_manager::~texture_manager() {
        GLsizei n = (GLsizei)textures.size();
        GLuint *tmp = new GLuint[n];
        int pos = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter) {
            tmp[pos++] = iter->second;
        }
        glGenTextures(n, tmp);
        
        delete [] tmp;
    }
    
    GLuint texture_manager::get_texture(const std::string& name) {
        auto pos = textures.find(name);
        if (pos != textures.end()) {
            return pos->second;
        }
        
        std::string path = directory + "/" + name;
        GLuint uiHandle = 0;
        if (PVRTTextureLoadFromPVR(path.c_str(), &uiHandle) != PVR_SUCCESS) {
            return 0;
        }
        
        textures[name] = uiHandle;
        return uiHandle;
    }

}