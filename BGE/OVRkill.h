// OVRkill.h

#pragma once

#include "OVR.h"
#include "FBO.h"

///@brief The OVRkill class is instantiated once globally and exists to push as much
/// code as possible out of the app skeleton main source file.
class OVRkill
{
public:
    OVRkill();
    virtual ~OVRkill();

    bool       SensorActive() const { return m_pSensor != NULL; }
    OVR::Quatf GetOrientation() const { return m_SFusion.GetOrientation(); }
    bool       GetStereoMode() const { return m_SConfig.GetStereoMode() == OVR::Util::Render::Stereo_LeftRight_Multipass; }
    const OVR::HMDInfo& GetHMD() const { return m_HMDInfo; }
    
    int GetRenderBufferWidth() const { return m_fboWidth; }
    int GetRenderBufferHeight() const { return m_fboHeight; }

    void InitOVR();
    void DestroyOVR();
    void CreateShaders();
    void CreateRenderBuffer();
    void BindRenderBuffer();
    void UnBindRenderBuffer();

    void PresentFbo();
    void PresentFbo_NoDistortion();
    void PresentFbo_PostProcessDistortion(const OVR::Util::Render::StereoEyeParams& eyeParams);
    

    enum DisplayMode
    {
        SingleEye,
        Stereo,
        StereoWithDistortion
    };

    void SetDisplayMode(DisplayMode);

protected:
    /// OVR hardware
    OVR::Ptr<OVR::DeviceManager>  m_pManager;
    OVR::Ptr<OVR::HMDDevice>      m_pHMD;
    OVR::Ptr<OVR::SensorDevice>   m_pSensor;
    OVR::SensorFusion             m_SFusion;
    OVR::HMDInfo                  m_HMDInfo;

    /// Stereo viewing parameters
    enum PostProcessType
    {
        PostProcess_None,
        PostProcess_Distortion
    };

    OVR::Util::Render::StereoConfig   m_SConfig;
    PostProcessType                   m_PostProcess;


    /// Render buffer for OVR distortion correction shader
    FBO m_renderBuffer;
    int m_fboWidth;
    int m_fboHeight;

    GLuint m_progRiftDistortion;
    GLuint m_progPresFbo;

    int m_windowWidth;
    int m_windowHeight;


private: // Disallow copy ctor and assignment operator
    OVRkill(const OVRkill&);
    OVRkill& operator=(const OVRkill&);
};
