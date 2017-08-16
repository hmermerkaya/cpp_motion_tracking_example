/*
 * Copyright 2014 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CPP_MOTION_TRACKING_EXAMPLE_TANGO_POINT_CLOUD_DRAWABLE_H_
#define CPP_MOTION_TRACKING_EXAMPLE_TANGO_POINT_CLOUD_DRAWABLE_H_

#include <jni.h>

#include <memory>
#include <mutex>
#include <string>

#include <tango_client_api.h>  // NOLINT
#include <tango-gl/util.h>

#include <vector>

#include <tango-gl/util.h>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
namespace tango_motion_tracking{

// PointCloudDrawable is responsible for the point cloud rendering.
class PointCloudDrawable {
 public:
  PointCloudDrawable();

  // Free all GL Resources, i.e, shaders, buffers.
  void DeleteGlResources();

  // Update current point cloud data.
  //
  // @param projection_mat: projection matrix from current render camera.
  // @param view_mat: view matrix from current render camera.
  // @param vertices: all vertices in this point cloud frame.
  //void Render(glm::mat4 projection_mat, glm::mat4 view_mat,
    //          const std::vector<float>& vertices);
 void Render(glm::mat4 projection_mat, glm::mat4 view_mat
              /*const std::pair <std::vector<float>,std::vector<float> > &verticesColors*/);
  void loadPointCloud( AAssetManager* mgr, const char *file_path);

 private:

  // Vertex buffer of the point cloud geometry.
  GLuint vertexColor_buffers_;
//  GLuint color_buffers_;
  // Shader to display point cloud.
  GLuint shader_program_;

  // Handle to vertex attribute value in the shader.
  GLuint vertices_handle_;

   GLuint colors_handle_;

  // Handle to the model view projection matrix uniform in the shader.
  GLuint mvp_handle_;
   std::pair <std::vector<float>,std::vector<float> >  *pointCloud_;
};
}  // namespace tango_point_cloud

#endif  // CPP_POINT_CLOUD_EXAMPLE_TANGO_POINT_CLOUD_POINT_CLOUD_DRAWABLE_H_
