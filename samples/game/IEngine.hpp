#pragma once

class IEngine
{
public:
    static IEngine* AllocateInstance();

    virtual void Init( const char* title, int width, int height ) = 0;
    virtual bool RunFrame() = 0;
    virtual void Shutdown() = 0;
};
