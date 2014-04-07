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

#include "camera.h"
#include <algorithm>

#define CAM_NEAR 1
#define CAM_FAR 10000

namespace mre {
    void camera::prepare() {
        PVRTVec3 pos(target);
        pos.z += distance;
        
        PVRTVec3 forward = pos - target;
        PVRTVec3 up(0.0, 1.0, 0.0);
        PVRTVec3 right(1.0, 0.0, 0.0);
        
        PVRTQUATERNION qu, qr, q;
        PVRTMatrixQuaternionRotationAxis(qu, up, up_rotation);
        PVRTMatrixQuaternionRotationAxis(qr, right, right_rotation);
        PVRTMatrixQuaternionMultiply(q, qu, qr);
        
        PVRTMat4 m;
        PVRTMatrixRotationQuaternion(m, q);
        
        position = forward * m + target;
        up = up * m;
        
        PVRTMat4 vpt = PVRTMat4::Translation(translation);
        view = vpt * PVRTMat4::LookAtRH(position, target, up);
        projection = PVRTMat4::PerspectiveFovRH(M_PI_4, aspect, CAM_NEAR, CAM_FAR, PVRTMat4::OGL, false);
    }
    
    void camera::setup(PVRTBOUNDINGBOX box) {
        up_rotation = 0;
        right_rotation = 0;
        translation *= 0;
        
        distance = std::max((double)box.Point[7].x, (double)box.Point[7].y);
        distance = std::max((double)distance, (double)box.Point[7].z);
        distance *= 2;
        
        target.x = (box.Point[0].x + box.Point[7].x) / 2;
        target.y = (box.Point[0].y + box.Point[7].y) / 2;
        target.z = (box.Point[0].z + box.Point[7].z) / 2;
    }
}