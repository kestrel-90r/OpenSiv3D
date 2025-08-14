//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025 kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

#include <Siv3D/WebcamInfo.hpp>
#include <Siv3D/Unicode.hpp>
#include <Siv3D/Array.hpp>

#if SIV3D_PLATFORM(ANDROID)
#include <camera/NdkCameraManager.h>
#endif

namespace s3d
{
    namespace System
    {
        Array<WebcamInfo> EnumerateWebcams()
        {
            Array<WebcamInfo> results;

#if SIV3D_PLATFORM(ANDROID)
            ACameraManager *camManager = ACameraManager_create();
            if (!camManager)
            {
                return results;
            }

            ACameraIdList *idList = nullptr;
            if (ACameraManager_getCameraIdList(camManager, &idList) != ACAMERA_OK || !idList)
            {
                ACameraManager_delete(camManager);
                return results;
            }

            for (int i = 0; i < idList->numCameras; ++i)
            {
                const char *id = idList->cameraIds[i];
                ACameraMetadata *metadata = nullptr;
                acamera_metadata_enum_android_lens_facing_t facingVal = ACAMERA_LENS_FACING_FRONT;

                if (ACameraManager_getCameraCharacteristics(camManager, id, &metadata) == ACAMERA_OK && metadata)
                {
                    ACameraMetadata_const_entry entry{};
                    if (ACameraMetadata_getConstEntry(metadata, ACAMERA_LENS_FACING, &entry) == ACAMERA_OK && entry.count > 0)
                    {
                        facingVal = static_cast<acamera_metadata_enum_android_lens_facing_t>(entry.data.u8[0]);
                    }
                }

                WebcamInfo info;
                info.cameraIndex = static_cast<uint32>(i);
                String label;
                switch (facingVal)
                {
                case ACAMERA_LENS_FACING_BACK:
                    label = U"Back";
                    break;
                case ACAMERA_LENS_FACING_EXTERNAL:
                    label = U"External";
                    break;
                case ACAMERA_LENS_FACING_FRONT:
                default:
                    label = U"Front";
                    break;
                }
                const String sid = Unicode::FromUTF8(id);
                info.name = U"Camera (" + label + U")";
                info.uniqueName = U"android:" + sid;
                results << info;

                if (metadata)
                {
                    ACameraMetadata_free(metadata);
                }
            }

            ACameraManager_deleteCameraIdList(idList);
            ACameraManager_delete(camManager);
#endif

            return results;
        }
    }
}
