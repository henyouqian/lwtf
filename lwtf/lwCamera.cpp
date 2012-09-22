#include "stdafx.h"
#include "lwCamera.h"
#include "lwApp.h"
#include "cml/cml.h"

namespace lw{

	Camera::Camera(){
		perspective(M_PI/3.f, 3.0f/2.0f, 1, 1000);
		lookat(10, 10, 10, 0, 0, 0, 0, 1, 0);
	}
    
    void Camera::getView(cml::Matrix4& mat){
        mat = _viewMatrix;
    }
    
    void Camera::getProj(cml::Matrix4& mat){
        mat = _projMatrix;
    }
    
    void Camera::getViewProj(cml::Matrix4& mat){
        mat = _projMatrix * _viewMatrix;
    }

	void Camera::lookat(
		float eyex,
		float eyey,
		float eyez,
		float centerx,
		float centery,
		float centerz,
		float upx,
		float upy,
		float upz){
			cml::matrix_look_at_RH(_viewMatrix, eyex, eyey, eyez,
				centerx, centery, centerz,
				upx, upy, upz);
	}

	void Camera::perspective(
		float fovy, 
		float aspect, 
		float zNear, 
		float zFar){
			//fovy = fovy*(float)M_PI/180.f;
			cml::matrix_perspective_yfov_RH(_projMatrix, fovy, aspect, zNear, zFar, cml::z_clip_neg_one);
	}

	void Camera::ortho(float left, float right, float bottom, float top, float znear, float zfar){
		cml::matrix_orthographic_RH(_projMatrix, left, right, bottom, top, znear, zfar, cml::z_clip_neg_one);
	}

} //namespace lw