
#include <Engine/Kernel.hpp>
#include "Endpoint/LobbyProtocol.hpp"
#include "Game/AccountServiceLocal.hpp"
#include <Content/Locator/SystemLocator.hpp>
#include <windows.h>

// -=(Undocumented)=-
class GameServer : public Core::Subsystem
{
public:

    // -=(Undocumented)=-
    GameServer(Ref<Context> Context)
        : Subsystem { Context }
    {
    }

    // -=(Undocumented)=-
    void OnDestroy()
    {
    }

    // -=(Undocumented)=-
    void OnInitialize()
    {
        mAccountService = NewPtr<Game::AccountServiceLocal>(GetContext());

        mServer = GetSubsystem<Network::Service>()->Listen("0.0.0.0", 7666);
        mServer->SetProtocol(NewPtr<Endpoint::LobbyProtocol>(mAccountService));
    }

    // -=(Undocumented)=-
    void OnTick() override
    {

    }

private:

    SPtr<Game::AccountServiceLocal> mAccountService;
    SPtr<Network::Server>           mServer;
};


int main()
{
    Engine::Kernel Kernel;
    Kernel.Initialize(decltype(Kernel)::Mode::Server, Engine::Properties {});
    Kernel.AddSubsystem<GameServer>();
    Kernel.GetSubsystem<GameServer>()->OnInitialize();

    while (true)
    {
        Kernel.Tick();
    }

    Kernel.GetSubsystem<GameServer>()->OnDestroy();


    return 0;
}