#include "VPad.hpp"
#include <Siv3D/Mouse/IMouse.hpp>
#include <Siv3D/Mouse/CMouse.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>

namespace s3d
{

    VPad* VPad::getInstance()
    {
        static std::once_flag initInstanceFlag;
        static VPad* instance = nullptr;
        
        std::call_once(initInstanceFlag, []() 
        {
            static std::unique_ptr<VPad> s_uniqueInstance = std::make_unique<VPad>();
            instance = s_uniqueInstance.get();
        });
        
        return instance;
    }

    void VPad::handleTouchEvent(int32 action, int32 pointerId, const Point& position)
    {
        auto* instance = getInstance();
        if (instance)
        {
            instance->HandleTouchEvent(action, pointerId, position);
        }
    }

    void VPad::init()
    {
        auto* instance = getInstance();
        if (instance)
        {
            instance->Init();
        }
    }

    void VPad::setScreenSize(int width, int height)
    {
        auto* instance = getInstance();
        if (instance)
        {
            instance->SetScreenSize(width, height);
        }
    }

    void VPad::setRealScreenSize(int width, int height)
    {
        auto* instance = getInstance();
        if (instance)
        {
            instance->SetRealScreenSize(width, height);
        }
    }

    void VPad::update()
    {
        auto* instance = getInstance();
        if (instance)
        {
            instance->Update();
        }
    }

    int16 VPad::addRegion(const RectF& region, int16 vk)
    {
        auto* instance = getInstance();
        if (instance)
        {
            return instance->AddRegion(region, vk);
        }
        return -1;
    }

    const s3d::Array<VPadRegion>& VPad::getRegions()
    {
        static const s3d::Array<VPadRegion> emptyArray;
        auto* instance = getInstance();
        if (instance)
        {
            return instance->GetRegions();
        }
        return emptyArray;
    }

    int16 VPad::getVKeyStatus(int16 vk)
    {
        auto* instance = getInstance();
        if (instance)
        {
            return instance->GetVKeyStatus(vk);
        }
        return VKOFF;
    }

    Vec2 VPad::getAnalogValue(int16 vk)
    {
        auto* instance = getInstance();
        if (instance)
        {
            return instance->GetAnalogValue(vk);
        }
        return Vec2{0, 0};
    }

    VPad::VPad() : 
        m_virtualScreenWidth(800), m_virtualScreenHeight(600),
        m_realScreenWidth(0), m_realScreenHeight(0),
        m_scale(1.0f), m_offsetX(0.0f), m_offsetY(0.0f)
    {
    }

    VPad::~VPad()
    {
        clearAllResources();
    }

    VPad::VPad(VPad&& other) noexcept
        : m_activePointers(std::move(other.m_activePointers))
        , m_vkTouchInfo(std::move(other.m_vkTouchInfo))
        , m_analogSticks(std::move(other.m_analogSticks))
        , m_regions(std::move(other.m_regions))
        , m_virtualScreenWidth(other.m_virtualScreenWidth)
        , m_virtualScreenHeight(other.m_virtualScreenHeight)
        , m_realScreenWidth(other.m_realScreenWidth)
        , m_realScreenHeight(other.m_realScreenHeight)
        , m_scale(other.m_scale)
        , m_offsetX(other.m_offsetX)
        , m_offsetY(other.m_offsetY)
    {
    }

    VPad& VPad::operator=(VPad&& other) noexcept
    {
        if (this != &other)
        {
            clearAllResources();
            
            m_activePointers = std::move(other.m_activePointers);
            m_vkTouchInfo = std::move(other.m_vkTouchInfo);
            m_analogSticks = std::move(other.m_analogSticks);
            m_regions = std::move(other.m_regions);
            m_virtualScreenWidth = other.m_virtualScreenWidth;
            m_virtualScreenHeight = other.m_virtualScreenHeight;
            m_realScreenWidth = other.m_realScreenWidth;
            m_realScreenHeight = other.m_realScreenHeight;
            m_scale = other.m_scale;
            m_offsetX = other.m_offsetX;
            m_offsetY = other.m_offsetY;
        }
        return *this;
    }

    void VPad::Init()
    {
        clearAllResources();
        m_virtualScreenWidth = Scene::Width();
        m_virtualScreenHeight = Scene::Height();
        updateScalingInfo();
    }

    void VPad::SetScreenSize(int32 width, int32 height)
    {
        if (m_virtualScreenWidth != width || m_virtualScreenHeight != height)
        {
            m_virtualScreenWidth = width;
            m_virtualScreenHeight = height;
            updateScalingInfo();
        }
    }

    void VPad::SetRealScreenSize(int32 width, int32 height)
    {
        m_realScreenWidth = width;
        m_realScreenHeight = height;
        updateScalingInfo();
    }

    void VPad::Update()
    {
        updateAllVKStatuses();
        updateAllAnalogSticks();

        auto* mouse = static_cast<CMouse*>(SIV3D_ENGINE(Mouse));
        if (!mouse) return;

        if (GetVKeyStatus(VKBTNL) == VKSTART)
        {
            mouse->updateButtonDown(0);
        }
        else if (GetVKeyStatus(VKBTNL) == VKEND)
        {
            mouse->updateButtonUp(0);
        }
        
        if (GetVKeyStatus(VKBTNR) == VKSTART)
        {
            mouse->updateButtonDown(1);
        }
        else if (GetVKeyStatus(VKBTNR) == VKEND)
        {
            mouse->updateButtonUp(1);
        }

        if (GetVKeyStatus(VKBTNM) == VKSTART)
        {
            mouse->updateButtonDown(2);
        }
        else if (GetVKeyStatus(VKBTNM) == VKEND)
        {
            mouse->updateButtonUp(2);
        }
        
    }

    int16 VPad::GetVKeyStatus(int16 vk) const
    {
        auto it = m_vkTouchInfo.find(vk);
        if (it != m_vkTouchInfo.end())
        {
            return it->second.currentStatus;
        }
        return VKOFF;
    }

    Vec2 VPad::GetAnalogValue(int16 vk) const
    {
        auto it = m_analogSticks.find(vk);
        if (it != m_analogSticks.end() && it->second.active)
        {
            return Vec2{ it->second.currentX, it->second.currentY };
        }
        return Vec2{ 0.0, 0.0 };
    }

    int16 VPad::AddRegion(const RectF& region, int16 vk)
    {
        RectF realRegion = toRealRect(region);
        VPadRegion vpadRegion(realRegion, region, vk);
        m_regions.push_back(vpadRegion);
        
        if (m_vkTouchInfo.find(vk) == m_vkTouchInfo.end())
        {
            m_vkTouchInfo[vk] = VKTouchInfo();
        }
        
        if (isAnalogStickVK(vk))
        {
            initializeAnalogSticks();
        }
        
        return vk;
    }

    const s3d::Array<VPadRegion>& VPad::GetRegions() const
    {
        return m_regions;
    }

    void VPad::HandleTouchEvent(int32 action, int32 pointerId, const Point& position)
    {
        HandleMultiTouchEvent(action, pointerId, (float)position.x, (float)position.y);
    }

    void VPad::HandleMultiTouchEvent(int32 action, int32 pointerId, float x, float y)
    {
        Vec2 virtualPos = toVirtualPos(x, y);

        switch (action)
        {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            {
                PointerInfo pointer;
                pointer.pointerId = pointerId;
                pointer.x = pointer.startX = x;
                pointer.y = pointer.startY = y;
                pointer.currentVK = pointer.startVK = findVKAtPosition(x, y);
                pointer.active = true;
                pointer.touchStartTime = std::chrono::steady_clock::now();

                m_activePointers[pointerId] = pointer;

                if (pointer.currentVK != VKNONE)
                {
                    auto it = m_vkTouchInfo.find(pointer.currentVK);
                    if (it != m_vkTouchInfo.end())
                    {
                        it->second.touchingPointers.insert(pointerId);
                        if (it->second.touchingPointers.size() == 1)
                        {
                            it->second.firstTouchTime = pointer.touchStartTime;
                        }
                    }
                }
                assignPointerToAnalogStick(pointerId, pointer);
            }
            break;

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            {
                auto it = m_activePointers.find(pointerId);
                if (it != m_activePointers.end())
                {
                    const PointerInfo &pointer = it->second;
                    if (pointer.currentVK != VKNONE)
                    {
                        auto vkIt = m_vkTouchInfo.find(pointer.currentVK);
                        if (vkIt != m_vkTouchInfo.end())
                        {
                            vkIt->second.touchingPointers.erase(pointerId);
                        }
                    }
                    unassignPointerFromAnalogStick(pointerId);
                    m_activePointers.erase(it);
                }
            }
            break;

            case AMOTION_EVENT_ACTION_MOVE:
            {
                auto it = m_activePointers.find(pointerId);
                if (it != m_activePointers.end())
                {
                    PointerInfo& currentPointer = it->second;
                    const int16 oldVK = currentPointer.currentVK;
                    
                    currentPointer.x = x;
                    currentPointer.y = y;
                    
                    bool isAnalogStickPointer = false;
                    for (auto& pair : m_analogSticks)
                    {
                        AnalogStickInfo& stick = pair.second;
                        if (stick.active && stick.assignedPointerId == pointerId)
                        {
                            updateStickValue(stick, x, y);
                            isAnalogStickPointer = true;
                            break;
                        }
                    }

                    if (!isAnalogStickPointer)
                    {
                        const int16 newVK = findVKAtPosition(x, y);
                        if (oldVK != newVK)
                        {
                            if (oldVK != VKNONE)
                            {
                                auto vkIt = m_vkTouchInfo.find(oldVK);
                                if (vkIt != m_vkTouchInfo.end())
                                {
                                    vkIt->second.touchingPointers.erase(pointerId);
                                    
                                    if (vkIt->second.touchingPointers.empty() && !isAnalogStickVK(oldVK) &&
                                       (vkIt->second.currentStatus == VKSTART || vkIt->second.currentStatus == VKMOVE))
                                    {
                                        vkIt->second.previousStatus = vkIt->second.currentStatus;
                                        vkIt->second.currentStatus = VKEND;
                                    }
                                }
                            }

                            if (newVK != VKNONE)
                            {
                                auto vkIt = m_vkTouchInfo.find(newVK);
                                if (vkIt != m_vkTouchInfo.end())
                                {
                                    vkIt->second.touchingPointers.insert(pointerId);
                                    if (vkIt->second.touchingPointers.size() == 1)
                                    {
                                        vkIt->second.firstTouchTime = std::chrono::steady_clock::now();
                                    }
                                }
                            }

                            currentPointer.currentVK = newVK;

                            if (isAnalogStickVK(newVK))
                            {
                                assignPointerToAnalogStick(pointerId, currentPointer);
                            }
                        }
                    }
                }
            }
            break;

            case AMOTION_EVENT_ACTION_CANCEL:
                m_activePointers.clear();
                for (auto &pair : m_vkTouchInfo)
                {
                    pair.second.touchingPointers.clear();
                }
                for (auto &pair : m_analogSticks)
                {
                    pair.second.active = false;
                    pair.second.assignedPointerId = -1;
                }
            break;
        }
    }

    Circle VPad::GetAnalogStickBaseCircle(int16 vk) const
    {
        auto it = m_analogSticks.find(vk);
        if (it != m_analogSticks.end())
        {
            const AnalogStickInfo& stick = it->second;
            return Circle{ stick.centerX, stick.centerY, stick.maxRadius };
        }
        return Circle{ 0, 0, 0 };
    }

    Circle VPad::GetAnalogStickCurrentCircle(int16 vk) const
    {
        auto it = m_analogSticks.find(vk);
        if (it != m_analogSticks.end() && it->second.active)
        {
            const AnalogStickInfo& stick = it->second;
            float currentX = stick.centerX + stick.currentX * stick.maxRadius;
            float currentY = stick.centerY + stick.currentY * stick.maxRadius;
            return Circle{ currentX, currentY, 15.0f };
        }
        return Circle{ 0, 0, 0 };
    }

    bool VPad::IsAnalogStickActive(int16 vk) const
    {
        auto it = m_analogSticks.find(vk);
        return (it != m_analogSticks.end() && it->second.active);
    }

    Vec2 VPad::toVirtualPos(float realX, float realY) const
    {
        return Vec2{
            (realX - m_offsetX) * m_scale,
            (realY - m_offsetY) * m_scale
        };
    }

    void VPad::clearAllResources()
    {
        m_activePointers.clear();
        m_vkTouchInfo.clear();
        m_analogSticks.clear();
        m_regions.clear();
    }

    void VPad::initializeAnalogSticks()
    {
        m_analogSticks.clear();

        for (const auto& region : m_regions)
        {
            if (region.vk == VKLSTICK || region.vk == VKRSTICK)
            {
                AnalogStickInfo stick;
                stick.vk = region.vk;
                stick.assignedPointerId = -1;
                stick.active = false;

                stick.centerX = region.x + region.w * 0.5f;
                stick.centerY = region.y + region.h * 0.5f;

                stick.maxRadius = std::min(region.w, region.h) * 0.4f;
                stick.deadZone = stick.maxRadius * 0.15f;

                stick.currentX = 0.0f;
                stick.currentY = 0.0f;

                m_analogSticks[region.vk] = stick;
            }
        }
    }

    bool VPad::isAnalogStickVK(int16 vk) const
    {
        return vk >= VKANALOG && vk <= VKRSTICK;
    }

    RectF VPad::toRealRect(const RectF& r) const
    {
        return RectF{
            r.x * (1.0f / m_scale) + m_offsetX,
            r.y * (1.0f / m_scale) + m_offsetY,
            r.w * (1.0f / m_scale),
            r.h * (1.0f / m_scale)
        };
    }

    int16 VPad::findVKAtPosition(float x, float y) const
    {
        for (const auto& region : m_regions)
        {
            if (inRect(region, x, y))
            {
                return region.vk;
            }
        }
        
        return VKNONE;
    }

    void VPad::assignPointerToAnalogStick(int32 pointerId, const PointerInfo& pointer)
    {
        if (pointer.currentVK == VKLSTICK || pointer.currentVK == VKRSTICK)
        {
            auto it = m_analogSticks.find(pointer.currentVK);
            if (it != m_analogSticks.end() && !it->second.active)
            {
                AnalogStickInfo& stick = it->second;
                stick.active = true;
                stick.assignedPointerId = pointerId;
                
                updateStickValue(stick, pointer.x, pointer.y);
            }
        }
    }

    void VPad::unassignPointerFromAnalogStick(int32 pointerId)
    {
        for (auto& pair : m_analogSticks)
        {
            AnalogStickInfo& stick = pair.second;
            if (stick.assignedPointerId == pointerId)
            {
                stick.active = false;
                stick.assignedPointerId = -1;
                stick.currentX = 0.0f;
                stick.currentY = 0.0f;
            }
        }
    }

    void VPad::reassignPointerToAnalogStick(int32 pointerId, const PointerInfo& pointer)
    {
        unassignPointerFromAnalogStick(pointerId);
        if (pointer.currentVK == VKLSTICK || pointer.currentVK == VKRSTICK)
        {
            assignPointerToAnalogStick(pointerId, pointer);
        }
    }

    void VPad::updateStickValue(AnalogStickInfo& stick, float x, float y)
    {
        float deltaX = x - stick.centerX;
        float deltaY = y - stick.centerY;
        float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);

        if (distance > stick.maxRadius)
        {
            deltaX = (deltaX / distance) * stick.maxRadius;
            deltaY = (deltaY / distance) * stick.maxRadius;
            distance = stick.maxRadius;
        }

        float normalizedValue = 0.0f;
        if (distance > stick.deadZone)
        {
            normalizedValue = (distance - stick.deadZone) / (stick.maxRadius - stick.deadZone);
            normalizedValue = std::min(normalizedValue, 1.0f);
        }

        if (normalizedValue > 0.0f)
        {
            stick.currentX = (deltaX / stick.maxRadius);
            stick.currentY = (deltaY / stick.maxRadius);
        }
        else
        {
            stick.currentX = 0.0f;
            stick.currentY = 0.0f;
        }
    }

    void VPad::updateAllVKStatuses()
    {
        for (auto& pair : m_vkTouchInfo)
        {
            int16 vk = pair.first;
            VKTouchInfo& vkInfo = pair.second;
            
            vkInfo.previousStatus = vkInfo.currentStatus;

            if (vkInfo.hasActiveTouches())
            {
                if (vkInfo.previousStatus == VKOFF)
                    vkInfo.currentStatus = VKSTART;
                else
                    vkInfo.currentStatus = VKMOVE;
            }
            else
            {
                if (vkInfo.previousStatus == VKMOVE || vkInfo.previousStatus == VKSTART)
                    vkInfo.currentStatus = VKEND;
                else
                    vkInfo.currentStatus = VKOFF;
            }

            for (auto& region : m_regions)
            {
                if (region.vk == vk)
                {
                    region.status = vkInfo.currentStatus;
                }
            }
        }
        
        for (auto& pair : m_analogSticks)
        {
            AnalogStickInfo& stick = pair.second;
            int16 vk = pair.first;
            
            if (stick.active && stick.assignedPointerId != -1)
            {
                auto pointerIt = m_activePointers.find(stick.assignedPointerId);
                if (pointerIt != m_activePointers.end())
                {
                    auto& vkInfo = m_vkTouchInfo[vk];
                    if (vkInfo.currentStatus == VKOFF)
                    {
                        vkInfo.currentStatus = VKSTART;
                        vkInfo.previousStatus = VKOFF;
                    }
                    else if (vkInfo.currentStatus == VKSTART)
                    {
                        vkInfo.currentStatus = VKMOVE;
                        vkInfo.previousStatus = VKSTART;
                    }
                    
                    for (auto& region : m_regions)
                    {
                        if (region.vk == vk)
                        {
                            region.status = vkInfo.currentStatus;
                        }
                    }
                }
                else
                {
                    stick.active = false;
                    stick.assignedPointerId = -1;
                    stick.currentX = 0.0f;
                    stick.currentY = 0.0f;

                    auto& vkInfo = m_vkTouchInfo[vk];
                    vkInfo.currentStatus = VKEND;
                    vkInfo.previousStatus = VKMOVE;

                    for (auto& region : m_regions)
                    {
                        if (region.vk == vk)
                        {
                            region.status = VKEND;
                        }
                    }
                }
            }
        }
    }

    void VPad::updateAllAnalogSticks()
    {
        for (auto& pair : m_analogSticks)
        {
            AnalogStickInfo& stick = pair.second;
            if (stick.active && stick.assignedPointerId != -1)
            {
                auto pointerIt = m_activePointers.find(stick.assignedPointerId);
                if (pointerIt != m_activePointers.end())
                {
                    const PointerInfo& pointer = pointerIt->second;
                    updateStickValue(stick, pointer.x, pointer.y);
                }

                else
                {
                    stick.active = false;
                    stick.assignedPointerId = -1;
                    stick.currentX = 0.0f;
                    stick.currentY = 0.0f;
                }
            }
        }
    }

    bool VPad::inRect(const VPadRegion &region, float x, float y) const
    {
        return x >= region.x && x < region.x + region.w &&
               y >= region.y && y < region.y + region.h;
    }

    void VPad::updateScalingInfo()
    {
        int32 physicalWidth = (m_realScreenWidth > 0) ? m_realScreenWidth :
                             Window::GetState().frameBufferSize.x;
        int32 physicalHeight = (m_realScreenHeight > 0) ? m_realScreenHeight : 
                              Window::GetState().frameBufferSize.y;
        
        float scaleToReal = std::min(
            static_cast<float>(physicalWidth) / static_cast<float>(m_virtualScreenWidth),
            static_cast<float>(physicalHeight) / static_cast<float>(m_virtualScreenHeight)
        );
        
        m_scale = 1.0f / scaleToReal;
        float renderWidth = m_virtualScreenWidth * scaleToReal;
        float renderHeight = m_virtualScreenHeight * scaleToReal;
        
        m_offsetX = (physicalWidth - renderWidth) / 2.0f;
        m_offsetY = (physicalHeight - renderHeight) / 2.0f;
        
    }

    VPad::VPadStickInfo VPad::GetStickInfo(int16 vk) const
    {
        VPadStickInfo info;

        for (const auto& region : m_regions)
        {
            if (region.vk == vk)
            {

                float radius = std::min(region.virtualRect.w, region.virtualRect.h) * 0.4f;

                info.baseCircle = Circle{
                    region.virtualRect.x + region.virtualRect.w * 0.5f,
                    region.virtualRect.y + region.virtualRect.h * 0.5f,
                    radius
                };

                auto analogValue = GetAnalogValue(vk);
                info.normalizedValue = analogValue;

                Vec2 stickPos = info.baseCircle.center.movedBy(
                    analogValue.x * radius * 0.8f,
                    analogValue.y * radius * 0.8f
                );

                info.knobCircle = Circle{ stickPos, radius * 0.4f };
                info.active = (GetVKeyStatus(vk) != VKOFF);

                return info;
            }
        }

        return info;
    }

    VPad::ButtonInfo VPad::GetButtonInfo(int16 vk) const
    {
        return GetButtonInfo(vk, nullptr, nullptr);
    }

    VPad::ButtonInfo VPad::GetButtonInfo(int16 vk, const std::function<String(int16)>& getLabelFunc,
                                   const std::function<std::pair<ColorF, ColorF>(int16)>& getColorsFunc) const
    {
        ButtonInfo info;

        for (const auto& region : m_regions)
        {
            if (region.vk == vk)
            {
                info.rect = region.virtualRect;
                info.isActive = (region.status != VKOFF);

                if (getLabelFunc)
                {
                    info.label = getLabelFunc(vk);
                }
                else
                {
                    // デフォルトのラベル
                    switch (vk)
                    {
                        case VKBTNA: info.label = U"A"; break;
                        case VKBTNB: info.label = U"B"; break;
                        case VKBTNX: info.label = U"X"; break;
                        case VKBTNY: info.label = U"Y"; break;
                        case VKLEFT: info.label = U"←"; break;
                        case VKRIGHT: info.label = U"→"; break;
                        case VKUP: info.label = U"↑"; break;
                        case VKDOWN: info.label = U"↓"; break;
                        case VKL1: info.label = U"L1"; break;
                        case VKL2: info.label = U"L2"; break;
                        case VKR1: info.label = U"R1"; break;
                        case VKR2: info.label = U"R2"; break;
                        case VK_START: info.label = U"START"; break;
                        case VKSELECT: info.label = U"SELECT"; break;

                        case VKBTNL: info.label = U"LMB"; break;
                        case VKBTNM: info.label = U"MMB"; break;
                        case VKBTNR: info.label = U"RMB"; break;
                        default: info.label = U"BTN"; break;
                    }
                }

                if (getColorsFunc)
                {
                    auto [active, inactive] = getColorsFunc(vk);
                    info.activeColor = active;
                    info.inactiveColor = inactive;
                }
                else
                {
                    switch (vk)
                    {
                        case VKBTNA: 
                            info.activeColor = ColorF{0.9, 0.2, 0.2, 0.8};
                            info.inactiveColor = ColorF{0.4, 0.1, 0.1, 0.5};
                            break;
                        case VKBTNB:
                            info.activeColor = ColorF{0.2, 0.9, 0.2, 0.8};
                            info.inactiveColor = ColorF{0.1, 0.4, 0.1, 0.5};
                            break;
                        case VKBTNX:
                            info.activeColor = ColorF{0.2, 0.2, 0.9, 0.8};
                            info.inactiveColor = ColorF{0.1, 0.1, 0.4, 0.5};
                            break;
                        case VKBTNY:
                            info.activeColor = ColorF{0.9, 0.9, 0.2, 0.8};
                            info.inactiveColor = ColorF{0.4, 0.4, 0.1, 0.5};
                            break;
                        default:
                            info.activeColor = ColorF{0.7, 0.7, 0.7, 0.8};
                            info.inactiveColor = ColorF{0.3, 0.3, 0.3, 0.5};
                            break;
                    }
                }
                
                break;
            }
        }
        
        return info;
    }

    void VPad::SetButtonStyle(int16 vk, const ButtonStyle& style)
    {
        m_buttonStyles[vk] = style;
    }

    void VPad::SetButtonStyle(int16 vk, const String& label, const ColorF& activeColor, const ColorF& inactiveColor)
    {
        m_buttonStyles[vk] = ButtonStyle{label, activeColor, inactiveColor};
    }

    VPad::ButtonStyle VPad::GetButtonStyle(int16 vk) const
    {
        auto it = m_buttonStyles.find(vk);
        if (it != m_buttonStyles.end())
        {
            return it->second;
        }
        
        // デフォルトスタイル
        ButtonStyle style;
        switch (vk)
        {
            case VKBTNA:  return ButtonStyle{U"A", ColorF{0.9, 0.2, 0.2, 0.8}, ColorF{0.4, 0.1, 0.1, 0.5}};
            case VKBTNB:  return ButtonStyle{U"B", ColorF{0.2, 0.9, 0.2, 0.8}, ColorF{0.1, 0.4, 0.1, 0.5}};
            case VKBTNX:  return ButtonStyle{U"X", ColorF{0.2, 0.2, 0.9, 0.8}, ColorF{0.1, 0.1, 0.4, 0.5}};
            case VKBTNY:  return ButtonStyle{U"Y", ColorF{0.9, 0.9, 0.2, 0.8}, ColorF{0.4, 0.4, 0.1, 0.5}};
            case VKLEFT:  return ButtonStyle{U"←", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5}};
            case VKRIGHT: return ButtonStyle{U"→", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5}};
            case VKUP:    return ButtonStyle{U"↑", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5}};
            case VKDOWN:  return ButtonStyle{U"↓", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5}};
            case VKL1:    return ButtonStyle{U"L1", ColorF{0.8, 0.5, 0.5, 0.8}, ColorF{0.4, 0.2, 0.2, 0.5}};
            case VKL2:    return ButtonStyle{U"L2", ColorF{0.8, 0.3, 0.3, 0.8}, ColorF{0.4, 0.15, 0.15, 0.5}};
            case VKR1:    return ButtonStyle{U"R1", ColorF{0.5, 0.8, 0.5, 0.8}, ColorF{0.2, 0.4, 0.2, 0.5}};
            case VKR2:    return ButtonStyle{U"R2", ColorF{0.3, 0.8, 0.3, 0.8}, ColorF{0.15, 0.4, 0.15, 0.5}};
            case VKLSTICK:return ButtonStyle{U"LS", ColorF{0.5, 0.5, 0.9, 0.8}, ColorF{0.2, 0.2, 0.4, 0.5}};
            case VKRSTICK:return ButtonStyle{U"RS", ColorF{0.5, 0.9, 0.5, 0.8}, ColorF{0.2, 0.4, 0.2, 0.5}};
            case VKBTNL:  return ButtonStyle{U"ML", ColorF{0.9, 0.3, 0.3, 0.8}, ColorF{0.4, 0.1, 0.1, 0.5}};
            case VKBTNM:  return ButtonStyle{U"MM", ColorF{0.3, 0.9, 0.3, 0.8}, ColorF{0.1, 0.4, 0.1, 0.5}};
            case VKBTNR:  return ButtonStyle{U"MR", ColorF{0.3, 0.3, 0.9, 0.8}, ColorF{0.1, 0.1, 0.4, 0.5}};
            case VK_START: return ButtonStyle{U"START", ColorF{0.7, 0.7, 0.7, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5}};
            case VKSELECT: return ButtonStyle{U"SELECT", ColorF{0.7, 0.7, 0.7, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5}};
            default:      return ButtonStyle{U"BTN", ColorF{0.7, 0.7, 0.7, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5}};
        }
        return style;
    }

    RectF VPad::GetButtonRect(int16 vk) const
    {
        return GetButtonInfo(vk).rect;
    }

    bool VPad::IsButtonActive(int16 vk) const
    {
        return GetVKeyStatus(vk) != VKOFF;
    }

}
