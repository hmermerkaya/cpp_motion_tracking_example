
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

#include <sstream>
#include <android/asset_manager.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "tango-motion-tracking/point_cloud_drawable.h"

namespace {
const std::string kPointCloudVertexShader =
    "precision mediump float;\n"
    "precision mediump int;\n"
    "attribute vec3 vertex;\n"
    "attribute vec4 colors;\n"
    "uniform mat4 mvp;\n"
    "varying vec4 v_color;\n"

    "void main() {\n"
    "   float w = 2.24;\n"
    "   mat3 A = mat3( vec3(1.0,0.0,0.0), vec3(0.0,cos(w),-sin(w)), vec3(0.0,sin(w),cos(w)));\n"
    "   vec4 vert4=vec4(A*vertex,1.0);\n"
    "   gl_Position = mvp*vert4;\n"
    "   gl_PointSize = 7.0;\n"
    "   v_color = colors;\n"
    "}\n";
const std::string kPointCloudFragmentShader =
    "precision mediump float;\n"
    "precision mediump int;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(v_color);\n"
    "}\n";

}  // namespace

namespace tango_motion_tracking {

PointCloudDrawable::PointCloudDrawable() {
  shader_program_ = tango_gl::util::CreateProgram(
      kPointCloudVertexShader.c_str(), kPointCloudFragmentShader.c_str());

  mvp_handle_ = glGetUniformLocation(shader_program_, "mvp");
  vertices_handle_ = glGetAttribLocation(shader_program_, "vertex");
  colors_handle_= glGetAttribLocation(shader_program_, "colors");
  glGenBuffers(1, &vertexColor_buffers_);
  // glGenBuffers(1, &color_buffers_);
}

void PointCloudDrawable::DeleteGlResources() {
  if (vertexColor_buffers_) {
    glDeleteBuffers(1, &vertexColor_buffers_);

  }
  if (shader_program_) {
    glDeleteShader(shader_program_);
  }
}

/*void PointCloudDrawable::Render(glm::mat4 projection_mat, glm::mat4 view_mat,
                                const std::vector<float>& vertices) {
  glUseProgram(shader_program_);
  mvp_handle_ = glGetUniformLocation(shader_program_, "mvp");
  // Calculate model view projection matrix.
  glm::mat4 mvp_mat = projection_mat * view_mat;
  glUniformMatrix4fv(mvp_handle_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(vertices_handle_);
  glVertexAttribPointer(vertices_handle_, 3, GL_FLOAT, GL_FALSE,
                        4 * sizeof(float), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_POINTS, 0, vertices.size() / 4);

  glUseProgram(0);
  tango_gl::util::CheckGlError("Pointcloud::Render()");
}*/
void PointCloudDrawable::Render(glm::mat4 projection_mat, glm::mat4 view_mat
                                /*const std::pair <std::vector<float>,std::vector<float> > &verticesColors*/) {
  glUseProgram(shader_program_);
  mvp_handle_ = glGetUniformLocation(shader_program_, "mvp");
  // Calculate model view projection matrix.
  glm::mat4 mvp_mat = projection_mat * view_mat;
  glUniformMatrix4fv(mvp_handle_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

  glBindBuffer(GL_ARRAY_BUFFER, vertexColor_buffers_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 7 * pointCloud_->first.size()/3,
               NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER,0,  sizeof(GLfloat)*pointCloud_->first.size(),pointCloud_->first.data());

  glBufferSubData(GL_ARRAY_BUFFER,sizeof(GLfloat)*pointCloud_->first.size(),sizeof(GLfloat)*pointCloud_->second.size(),pointCloud_->second.data());

  glEnableVertexAttribArray(vertices_handle_);
  glVertexAttribPointer(vertices_handle_, 3, GL_FLOAT, GL_FALSE,
                        0, nullptr);

  glEnableVertexAttribArray(colors_handle_);
  glVertexAttribPointer(colors_handle_, 4, GL_FLOAT, GL_FALSE,
                        0, BUFFER_OFFSET(sizeof(GLfloat)*pointCloud_->first.size()));
  glBindBuffer(GL_ARRAY_BUFFER, 0);



  glDrawArrays(GL_POINTS, 0, pointCloud_->first.size() / 3);

  glUseProgram(0);
  tango_gl::util::CheckGlError("Pointcloud::Render()");
}

  void PointCloudDrawable::loadPointCloud( AAssetManager* mgr, const char *file_path) {
        // std::pair<std::vector<float>,std::vector<float>> emptyPair;
        std::vector<float> colors, points;
        AAsset* asset = AAssetManager_open(mgr, file_path, AASSET_MODE_BUFFER);
        if (asset == NULL) {
            LOGE("Error opening asset %s", file_path);
            return ;
        }
      /*  off_t length;
        off_t start;
        int fd = AAsset_openFileDescriptor(asset, &start, &length);
        lseek(fd, start, SEEK_CUR);
        FILE* file = fdopen(fd, "r");*/


        size_t fileLength = AAsset_getLength(asset);

        // Allocate memory to read your file
        char* fileContent = new char[fileLength+1];

        // Read your file
        AAsset_read(asset, fileContent, fileLength);
        // For safety you can add a 0 terminating character at the end of your file ...
        fileContent[fileLength] = '\0';
        //std::cout<<"Hello Hamit "<<std::endl;
       // std::cout<<fileContent<<std::endl;
       //  LOGI("fileContent %d %c",fileLength,fileContent[340]);

       std::stringstream ss(fileContent);
       std::string to;
       char *end;
          if (fileContent != NULL) {
              bool parsed = false;
              while (std::getline(ss, to, '\n')) {
                //  char *line=  (char*) to.c_str();
                 //LOGI("tooo %s", line);
                  if (!parsed && strstr(to.c_str(), "DATA ascii") != NULL) {
                      parsed = true;
                      continue;
                  } else if (!parsed) continue;
                  else if (strstr(to.c_str(), "nan") != NULL) continue;

                  std::stringstream strline(to);
                  int i=0;
                 // LOGI("float_f begin");
                  while (strline.good() && i < 4){
                      std::string str;
                      strline >> str;
                      float f= atof(str.c_str());
               //       LOGI(" %.10e ", f);
                      if (i == 3) {
                          uint32_t rgb = *reinterpret_cast<int *>(&f);
                          uint8_t r = (rgb >> 16) & 0x0000ff;
                          colors.push_back(r / 255.0f);
                          uint8_t g = (rgb >> 8) & 0x0000ff;
                          colors.push_back(g / 255.0f);
                          uint8_t b = (rgb) & 0x0000ff;
                          colors.push_back(b / 255.0f);
                          colors.push_back(1.0f);
                      } else points.push_back(f);
                    ++i;
                }

              }
          }


        delete [] fileContent;

      pointCloud_=  new std::pair <std::vector<float>,std::vector<float> > (std::make_pair(points,colors) );

    }

}  // namespace tango_point_cloud
