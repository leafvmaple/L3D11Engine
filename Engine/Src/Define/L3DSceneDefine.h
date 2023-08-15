#pragma once

#include "L3DCameraDefine.h"

class L3DRenderUnit;

struct RENDER_TO_TARGE_PARAM
{
    CAMERA_INFO CameraInfo;
};

struct SCENE_RENDER_CONTEXT
{
    RENDER_TO_TARGE_PARAM RenderToTargeParam;
};

struct SCENE_RENDER_QUEUES
{
    std::vector<L3DRenderUnit*> GBufferQueue;
    std::vector<L3DRenderUnit*> TransparencyQueue;
};