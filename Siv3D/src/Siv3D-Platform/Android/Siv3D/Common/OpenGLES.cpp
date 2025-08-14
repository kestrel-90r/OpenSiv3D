//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2022-2025 kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

#include "OpenGLES.hpp"
#include <Siv3D/Unicode.hpp>
#include <Siv3D/EngineLog.hpp>

namespace s3d
{
    void CheckOpenGLESError()
    {
        size_t limitter = 0;
        GLenum err;

        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::string errorString;
            switch (err)
            {
            case GL_INVALID_ENUM:
                errorString = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorString = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorString = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                errorString = "GL_OUT_OF_MEMORY";
                break;
            default:
                errorString = "Unknown Error";
                break;
            }

            LOG_ERROR(U"OpenGL ES Error: 0x{:x} ({})"_fmt(err, Unicode::Widen(errorString)));

            if (++limitter > 30)
            {
                LOG_ERROR(U"OpenGL ES error report interrupted.");
                break;
            }
        }
    }
}