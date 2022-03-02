#pragma once
#include "Utils.hpp"
#include "Icomponent.hpp"
#include "GeoMath.hpp"

#include <vector>

class SceneNode{
public:
    SceneNode(uint32_t nodeIndex, SceneNode* pParentNode);

    virtual void OnUpdate();
    virtual void OnRender() = 0;

    virtual void AddChild(std::unique_ptr<SceneNode>&& childNode);
    virtual void AddComponent(std::shared_ptr<IComponent>& component);

    virtual void SetMatrix(
        const GeoMath::Matrix4f* S, 
        const GeoMath::Matrix4f* R,
        const GeoMath::Matrix4f* T
    );
    
    virtual GeoMath::Matrix4f GetTransform() const { return r * t * m_parentNode->GetTransform(); };

    const GeoMath::Matrix4f& GetT()         const { return t; }
    const GeoMath::Matrix4f& GetToWorld()   const { return m_toWorld; }

    bool IsVisible() const { return m_isVisible; }
    bool IsDirty()   const { return m_isDirty; }

    void SetVisibility(const bool isVisible){ m_isVisible = isVisible; }
    void Pollute();

    static void SetDirtyCount(uint8_t dirtyCount){ DirtyCount = dirtyCount; }
protected:
    bool     m_isVisible;
    bool     m_isDirty;
    uint32_t m_nodeIndex;
    uint8_t  m_dirtyCount;

    GeoMath::Matrix4f s, r, t;
    GeoMath::Matrix4f m_toWorld;

    SceneNode* m_parentNode;

    using SceneNodeList = std::vector<std::unique_ptr<SceneNode>>;
    using ComponentList = std::vector<std::shared_ptr<IComponent>>;
    SceneNodeList m_childNodes;
    ComponentList m_components;

    inline static uint8_t DirtyCount = 1;
};

class Scene : public SceneNode{
public:
    Scene() : SceneNode(0, nullptr){};

    virtual void OnUpdate() override;
    virtual void OnRender() override {}

    virtual GeoMath::Matrix4f GetTransform() const override { return r * t; }
};

class CameraNode : public SceneNode{
public:
    explicit CameraNode(uint32_t nodeIndex, SceneNode* pParentNode);
    virtual void OnUpdate() override;
    virtual void Input(const uint16_t input, const int16_t x = 0, const int16_t y = 0);

    virtual void SetMatrix(
        const GeoMath::Matrix4f* S, 
        const GeoMath::Matrix4f* R,
        const GeoMath::Matrix4f* T
    ) override;

    void SetLens(
        const float zNear, const float zFar,
        const float aspect, const float fovY
    );

    GeoMath::Matrix4f GetView() const;
    const GeoMath::Matrix4f& GetProj() const;

protected:
    float m_nearZ;
    float m_farZ;
    float m_aspect;
    float m_fov;

    float m_pitch;
    float m_yaw;

    GeoMath::Matrix4f m_proj;
};

class FirstPersonCamera : public CameraNode{
public:
    explicit FirstPersonCamera(uint32_t nodeIndex, SceneNode* pParentNode);
    virtual void Input(const uint16_t input, const int16_t x = 0, const int16_t y = 0) override;

    void SetParentVisibility(const bool isVisible);

};

class ThirdPersonCamera : public CameraNode{
public:
    explicit ThirdPersonCamera(uint32_t nodeIndex, SceneNode* pParentNode);
    virtual void OnUpdate() override;
    virtual void Input(const uint16_t input, const int16_t x = 0, const int16_t y = 0) override;

protected:
    float m_distance;
    float m_minDistance;
    float m_maxDistance;
};