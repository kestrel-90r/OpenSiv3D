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

#pragma once
#include <Siv3D/Common.hpp>
#include <Siv3D/Array.hpp>
#include <Siv3D/PointVector.hpp>
#include <Siv3D/Pentablet/IPentablet.hpp>
#include <android/input.h>

namespace s3d
{
    class CPentablet final : public ISiv3DPentablet
    {
    public:
        CPentablet();

        ~CPentablet() override;

        void init() override;

        void update() override;

        bool isAvailable() override;

        const PentabletState &getState() override;

        bool handleInputEvent(AInputEvent *event);

    private:
        bool m_initialized = false;
        bool m_stylusAvailable = false;

        PentabletState m_state;

        bool m_stylusDown = false;
        Point m_pos{0, 0};
    };
}