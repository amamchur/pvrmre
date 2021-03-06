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

#ifndef PVRMRE_TEXTURE_BUFFER_H
#define PVRMRE_TEXTURE_BUFFER_H

#include "OGLES2Tools.h"

namespace mre {
    class texture_buffer {
    private:
        GLsizei width;
        GLsizei height;
        GLuint texture;
        GLuint render_buffer;
        GLuint frame_buffer;
        bool auto_delete_texture;
    public:
        texture_buffer(GLsizei width, GLsizei height, bool auto_delete_texture = false);
        ~texture_buffer();
    };
}

#endif
