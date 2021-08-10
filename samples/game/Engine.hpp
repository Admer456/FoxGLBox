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
