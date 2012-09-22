#ifndef __LW_CAMERA_H__
#define __LW_CAMERA_H__

namespace lw{

	class Camera{
	public:
		Camera();
		
		void lookat(float eyex, float eyey, float eyez,
					float centerx, float centery, float centerz,
					float upx, float upy, float upz);
		void perspective(float fovy, float aspect, float zNear, float zFar);
		void ortho(float left, float right, float bottom, float top, float znear, float zfar);
        
        void getView(cml::Matrix4& mat);
		void getProj(cml::Matrix4& mat);
        void getViewProj(cml::Matrix4& mat);
		
	private:
		cml::Matrix4 _viewMatrix;
		cml::Matrix4 _projMatrix;
	};

} //namespace lw



#endif //__LW_CAMERA_H__