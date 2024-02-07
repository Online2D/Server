
#include <Engine/Kernel.hpp>
#include "Endpoint/LobbyProtocol.hpp"
#include "Game/AccountServiceLocal.hpp"

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
        mServerLobbyProtocol = NewPtr<Endpoint::LobbyProtocol>(mAccountService);

        mServer = GetSubsystem<Network::Service>()->Listen(1000, "0.0.0.0", "7666");
        mServer->Attach(mServerLobbyProtocol);

    }

    // -=(Undocumented)=-
    void OnTick() override
    {
        mServer->Flush();
    }

private:

    SPtr<Game::AccountServiceLocal> mAccountService;
    SPtr<Network::Server>           mServer;
    SPtr<Endpoint::LobbyProtocol>   mServerLobbyProtocol;
};


int main()
{
    Engine::Kernel Kernel;
    Kernel.Initialize(Engine::Properties {});
    Kernel.AddSubsystem<GameServer>();
    Kernel.GetSubsystem<GameServer>()->OnInitialize();

    while (true)
    {
        Kernel.Tick();
    }

    Kernel.GetSubsystem<GameServer>()->OnDestroy();


    return 0;
}