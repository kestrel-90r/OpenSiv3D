//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025      kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

#include <Siv3D/Common.hpp>
#include <Siv3D/EngineLog.hpp>
#include "CPentablet.hpp"

namespace s3d
{
    CPentablet::CPentablet()
    {
        // コンストラクタ
    }

    CPentablet::~CPentablet()
    {
        LOG_SCOPED_TRACE(U"CPentablet::~CPentablet()");
    }

    void CPentablet::init()
    {
        LOG_SCOPED_TRACE(U"CPentablet::init()");
        m_initialized = true;
    }

    void CPentablet::update()
    {
    }

    bool CPentablet::isAvailable()
    {
        return m_initialized && m_stylusAvailable;
    }

    const PentabletState &CPentablet::getState()
    {
        return m_state;
    }

    bool CPentablet::handleInputEvent(AInputEvent *event)
    {
        if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION)
        {
            return false;
        }

        int32_t toolType = AMotionEvent_getToolType(event, 0);
        if (toolType != AMOTION_EVENT_TOOL_TYPE_STYLUS)
        {
            return false;
        }

        m_stylusAvailable = true;

        int32_t action = AMotionEvent_getAction(event);
        int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;

        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        m_pos.set(static_cast<int32>(x), static_cast<int32>(y));

        switch (actionMasked)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_MOVE:
        {
            m_state.pressure = AMotionEvent_getPressure(event, 0);
            m_state.supportPressure = true;

            const float tilt = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_TILT, 0);
            const float orientation = AMotionEvent_getAxisValue(event,
                                                                AMOTION_EVENT_AXIS_ORIENTATION, 0);

            if (tilt != 0.0f || orientation != 0.0f)
            {
                m_state.supportOrientation = true;
                m_state.azimuth = orientation * 180.0 / Math::Pi;
                m_state.altitude = 90.0 - (tilt * 180.0 / Math::Pi);
            }

            // tangentPressure と twist は Android では未サポート
            m_state.tangentPressure = 0.0;
            m_state.twist = 0.0;
            m_state.supportTangentPressure = false;
            break;
        }

        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_CANCEL:
            m_state.pressure = 0.0;
            m_state.tangentPressure = 0.0;
            m_state.azimuth = 0.0;
            m_state.altitude = 0.0;
            m_state.twist = 0.0;
            break;
        }

        return true;
    }
}