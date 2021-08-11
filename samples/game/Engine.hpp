#pragma once

#include <vector>
#include "Vector.hpp"

namespace Entities
{
    class GameEntity;
}

class Engine : public IEngine
{
public:
    void                Init( const char* title, int width, int height ) override;
    bool                RunFrame() override;
    void                Shutdown() override;

    void                Print( const char* string );

    RenderModelHandle   GetModel( const char* modelPath );
    IRenderWorld*       GetRenderWorld()
    {
        return renderWorld;
    }

    void                SubmitRenderView( const RenderView& view, bool isMain = false );
    void                GetViewport( int& width, int& height )
    {
        width = windowWidth;
        height = windowHeight;
    }

private:
    void                CreateGameEntities();

    template<typename EntityClass>
    EntityClass*        CreateEntity( fglVector position, fglVector angles, RenderModelHandle modelHandle );

private:
    RenderView          mainView;

    IRenderWorld*       renderWorld{ nullptr };
    IRenderSystem*      renderSystem{ nullptr }; // mustn't be deleted!
    int                 windowWidth{ 0 };
    int                 windowHeight{ 0 };

    fglVector           gravityForce{ 0.0f, 0.0f, -9.81f };
    float               frameTime{ 0.016f };
    float               timeScale{ 1.0f };

    SDL_Window*         window{ nullptr };
    
    Entities::GameEntity* gameEntities[Entities::MaxGameEntities];
};

inline Engine* gEngine = nullptr;

/*
Copyright (c) 2021 Admer456

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
