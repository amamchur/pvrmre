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

#ifndef PVRMRE_CAMERA_H
#define PVRMRE_CAMERA_H

#include "OGLES2Tools.h"

namespace mre {
    class camera {
    public:
        double distance;
        double aspect;
        double up_rotation;
        double right_rotation;
        PVRTVec3 position;
        PVRTVec3 target;
        PVRTVec3 translation;
        
        PVRTMat4 view;
        PVRTMat4 projection;
        
        void prepare();
        void setup(PVRTBOUNDINGBOX bounding_box);
    };
}

#endif
