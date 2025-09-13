#pragma once
#include <Siv3D/Common/Siv3DEngine.hpp>
#include <Siv3D/Window/CWindow.hpp>
#include <Siv3D/EngineLog.hpp>
#include <Siv3D.hpp>

#include <android/log.h>
#include <android/input.h>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <cmath>
#include <mutex>
#include <memory>

namespace s3d
{
    enum : int16
    {
        VKPOINT = 0,
        VKLEFT,
        VKRIGHT,
        VKUP,
        VKDOWN,
        VKBTNA,  // A
        VKBTNB,  // B
        VKBTNX,  // X
        VKBTNY,  // Y
        VKL1,
        VKL2,
        VKR1,
        VKR2,
        VK_START,
        VKSELECT,

        VKANALOG = 100,
        VKLSTICK,
        VKRSTICK,

        VKBTNL,  // マウスLボタン
        VKBTNM,  // マウスMボタン
        VKBTNR,  // マウスRボタン

        VKNONE = -1
    };

    enum : int16
    {
        VKOFF = 0, VKSTART = 1, VKMOVE = 2, VKEND = 3, VKSTOP = 4
    };

    struct VPadRegion : public s3d::RectF
    {
        RectF virtualRect;
        int16 vk;
        int16 status;
        bool active;
        float alpha;

        VPadRegion() : s3d::RectF(0, 0, 0, 0), virtualRect(0, 0, 0, 0), 
                      vk(VKNONE), status(VKOFF), active(true), alpha(1.0f) {}
        VPadRegion(const s3d::RectF& realRect, const s3d::RectF& virtRect, int16 vk_, int16 status_ = VKOFF)
            : s3d::RectF(realRect), virtualRect(virtRect), vk(vk_), status(status_), active(true), alpha(1.0f) {}
    };

    struct PointerInfo
    {
        int32 pointerId;
        float x, y;
        float startX, startY;
        int16 currentVK;
        int16 startVK;
        bool active;
        std::chrono::steady_clock::time_point touchStartTime;

        PointerInfo() : pointerId(-1), x(0), y(0), startX(0), startY(0),
                        currentVK(VKNONE), startVK(VKNONE), active(false) {}
    };

    struct VKTouchInfo
    {
        std::unordered_set<int32> touchingPointers;
        int16 currentStatus;
        int16 previousStatus;
        std::chrono::steady_clock::time_point firstTouchTime;

        bool hasActiveTouches() const { return !touchingPointers.empty(); }
        int touchCount() const { return touchingPointers.size(); }
        
        VKTouchInfo() : currentStatus(VKOFF), previousStatus(VKOFF) {}
    };

    struct AnalogStickInfo
    {
        int32 assignedPointerId;
        int16 vk;
        float centerX, centerY;
        float currentX, currentY;
        float deadZone;
        float maxRadius;
        bool active;

        AnalogStickInfo() : assignedPointerId(-1), vk(VKNONE), centerX(0), centerY(0),
                            currentX(0), currentY(0), deadZone(0.1f),
                            maxRadius(50.0f), active(false) {}
    };

    struct PointerPair
    {
        int32 pointerId;
        PointerInfo info;
        PointerPair() : pointerId(-1) {}
        PointerPair(int32 id, const PointerInfo& i) : pointerId(id), info(i) {}
    };

    struct VKTouchPair
    {
        int16 vk;
        VKTouchInfo info;
        VKTouchPair() : vk(VKNONE) {}
        VKTouchPair(int16 key, const VKTouchInfo& i) : vk(key), info(i) {}
    };

    struct AnalogStickPair
    {
        int16 vk;
        AnalogStickInfo info;
        AnalogStickPair() : vk(VKNONE) {}
        AnalogStickPair(int16 key, const AnalogStickInfo& i) : vk(key), info(i) {}
    };

    class VPad
    {
    public:
        struct ButtonStyle 
        {
            String label;
            ColorF activeColor;
            ColorF inactiveColor;

            ButtonStyle() : label(U""),
                            activeColor(0.7, 0.7, 0.7, 0.8),
                            inactiveColor(0.3, 0.3, 0.3, 0.5) {}

            ButtonStyle(const String& lbl, const ColorF& active, const ColorF& inactive)
                    : label(lbl), activeColor(active), inactiveColor(inactive) {}
        };

    private:
        std::unordered_map<int32, PointerInfo> m_activePointers;
        std::unordered_map<int16, VKTouchInfo> m_vkTouchInfo;
        std::unordered_map<int16, AnalogStickInfo> m_analogSticks;
        s3d::Array<VPadRegion> m_regions;
        std::unordered_map<int16, ButtonStyle> m_buttonStyles;

        int32 m_virtualScreenWidth, m_virtualScreenHeight;
        int32 m_realScreenWidth, m_realScreenHeight;
        float m_scale;
        float m_offsetX, m_offsetY;

        static void handleTouchEvent(int32 action, int32 pointerId, const Point& position);
        static void init();
        static void setScreenSize(int width, int height);
        static void setRealScreenSize(int width, int height);
        static void update();
        static int16 addRegion(const RectF& region, int16 vk);
        static const s3d::Array<VPadRegion>& getRegions();
        static int16 getVKeyStatus(int16 vk);
        static Vec2 getAnalogValue(int16 vk);

        Vec2 toVirtualPos(float realX, float realY) const;
        void clearAllResources();
        void initializeAnalogSticks();
        bool isAnalogStickVK(int16 vk) const;
        RectF toRealRect(const RectF& r) const;
        int16 findVKAtPosition(float x, float y) const;
        void assignPointerToAnalogStick(int32 pointerId, const PointerInfo& pointer);
        void unassignPointerFromAnalogStick(int32 pointerId);
        void reassignPointerToAnalogStick(int32 pointerId, const PointerInfo& pointer);
        void updateStickValue(AnalogStickInfo& stick, float x, float y);
        void updateAllVKStatuses();
        void updateAllAnalogSticks();
        bool inRect(const VPadRegion &region, float x, float y) const;
        void updateScalingInfo();
    
        void forwardTouchToCursor(int32 pointerId, float x, float y);
        void forwardTouchMoveToCursor(int32 pointerId, float x, float y);
        void forwardTouchEndToCursor(int32 pointerId);
        bool isFirstCursorPointer(int32 pointerId) const;
    
    public:

        static VPad* getInstance();

        VPad();
        ~VPad();

        VPad(const VPad&) = delete;
        VPad& operator=(const VPad&) = delete;

        VPad(VPad&& other) noexcept;
        VPad& operator=(VPad&& other) noexcept;

        void Init();
        void SetScreenSize(int32 width, int32 height);
        void SetRealScreenSize(int32 width, int32 height);
        void Update();
        void Draw( double round = 20.0 ) const;
        int16 GetVKeyStatus(int16 vk) const;
        Vec2 GetAnalogValue(int16 vk) const;
        int16 AddRegion(const RectF& region, int16 vk);
        const s3d::Array<VPadRegion>& GetRegions() const;
        void HandleTouchEvent(int32 action, int32 pointerId, const Point& position);
        void HandleMultiTouchEvent(int32 action, int32 pointerId, float x, float y);
        Circle GetAnalogStickBaseCircle(int16 vk) const;
        Circle GetAnalogStickCurrentCircle(int16 vk) const;
        bool IsAnalogStickActive(int16 vk) const;

        struct VPadStickInfo 
		{
            Vec2 normalizedValue;
            Circle baseCircle;
            Circle knobCircle;
            String label;
            bool active;
            
            VPadStickInfo() : normalizedValue(0, 0), baseCircle(0, 0, 0), 
                             knobCircle(0, 0, 0), label(U""), active(false) {}
        };

        struct ButtonInfo 
		{
            RectF rect;
            String label;
            ColorF activeColor;
            ColorF inactiveColor;
            bool isActive;
            
            ButtonInfo() : rect(0, 0, 0, 0), label(U""), 
                          activeColor(0.7, 0.7, 0.7, 0.8), 
                          inactiveColor(0.3, 0.3, 0.3, 0.5), 
                          isActive(false) {}
        };

        VPadStickInfo GetStickInfo(int16 vk) const;
        ButtonInfo GetButtonInfo(int16 vk) const;
        ButtonInfo GetButtonInfo(int16 vk, const std::function<String(int16)>& getLabelFunc,
                               const std::function<std::pair<ColorF, ColorF>(int16)>& getColorsFunc) const;

        ButtonStyle GetButtonStyle(int16 vk) const;

        void SetButtonStyle(int16 vk, const ButtonStyle& style);
        void SetButtonStyle(int16 vk, const String& label, const ColorF& activeColor, const ColorF& inactiveColor);
        RectF GetButtonRect(int16 vk) const;
        bool IsButtonActive(int16 vk) const;
    };
}