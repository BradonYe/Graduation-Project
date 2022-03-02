#pragma once
#include "CommandQueue.hpp"
#include "Device.hpp"
#include "d3dx12.h"

#include <cstdint>
#include <memory>

struct FrameResource{
    FrameResource() 
        : fence(0)
    {}

    uint64_t                      fence;
    ComPtr<ID3D12Resource>        renderTarget;
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
};

class RenderResource{
public:
    RenderResource(uint8_t frameCount, uint16_t width, uint16_t height, HWND hWnd);

    ~RenderResource(){
        m_commandQueue->Flush();
    }

    RenderResource(RenderResource&&) = delete;
    RenderResource(const RenderResource&) = delete;
    RenderResource& operator=(RenderResource&&) = delete;
    RenderResource& operator=(const RenderResource&) = delete;

    void OnUpdate(){
        m_frameIndex = (m_frameIndex + 1) % m_frameCount;
        m_commandQueue->WaitForFenceValue(m_frameResources[m_frameIndex].fence);
        m_cmdList = m_commandQueue->GetCommandList();
    }

    void OnRender(ComPtr<ID3D12GraphicsCommandList2>& cmdList){
        m_frameResources[m_frameIndex].fence = m_commandQueue->ExecuteCommandList(cmdList);
        m_dxgiSwapChain->Present(1, 0);
    }

    void OnResize(){ m_commandQueue->Flush(); }

    void Flush(){ m_commandQueue->Flush(); }

    FrameResource& GetFrameResource(){ return m_frameResources[m_frameIndex]; }

    ComPtr<ID3D12Device8> GetDevice() { return m_device->DxDevice(); }
    ComPtr<ID3D12GraphicsCommandList2> GetCommandList(){ return m_cmdList; }

    uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2>& cmdList){
        return m_commandQueue->ExecuteCommandList(cmdList);
    }

private:
    HWND                               m_wnd;
    uint8_t                            m_frameIndex;
    uint8_t                            m_frameCount;
    uint16_t                           m_width;
    uint16_t                           m_height;
    uint32_t                           m_rtvDescriptorSize;

    std::unique_ptr<Device>            m_device;
    std::unique_ptr<CommandQueue>      m_commandQueue;
    std::unique_ptr<FrameResource[]>   m_frameResources;

    ComPtr<IDXGISwapChain3>            m_dxgiSwapChain;
    ComPtr<ID3D12GraphicsCommandList2> m_cmdList;

    ComPtr<ID3D12DescriptorHeap>       m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap>       m_descHeap;
};
