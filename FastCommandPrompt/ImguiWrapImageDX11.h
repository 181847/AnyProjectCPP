#pragma once

#include <d3d11.h>

/*!
    \brief a wrapper class help to create texture from pixel datas.
*/
class ImguiWrapImageDX11
{
public:
    /*!
        \brief the texture in dx11
    */
    ID3D11Texture2D* m_pTexture = nullptr;

    /*!
        \brief SRV of the texture
    */
    ID3D11ShaderResourceView* m_pSRV = nullptr;

public:
    ImguiWrapImageDX11();
    ImguiWrapImageDX11(ImguiWrapImageDX11&& other);
    ImguiWrapImageDX11(const ImguiWrapImageDX11&) = delete;
    ImguiWrapImageDX11& operator=(const ImguiWrapImageDX11&) = delete;
    ~ImguiWrapImageDX11();

    /*!
        \brief create texture data in dx11,
        release previous data if have.
    */
    bool SetImageRawData(ID3D11Device* pDevice, const unsigned char * pRawData, unsigned int WIDTH, unsigned int HEIGHT);
    
    /*!
        \brief get the shader resource view of the texture.
    */
    ID3D11ShaderResourceView* GetSRV();

    /*!
        \brief release all the data, and set pointer to nullptr.
    */
    void Release();
};

