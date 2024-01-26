
#include <Engine/Kernel.hpp>

// -=(Undocumented)=-
class GameServer : public EnableSmartPointer<GameServer>, public Core::Subsystem, public Network::Protocol
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
        mEndpoint = nullptr;
    }

    // -=(Undocumented)=-
    void OnInitialize()
    {
        mEndpoint = GetSubsystem<Network::Service>()->Listen(1000, "0.0.0.0", "7666");
        mEndpoint->Attach(shared_from_this());
    }

    // -=(Undocumented)=-
    void OnTick() override
    {

    }

private:

    // -=(Undocumented)=-
    void OnAttach(ConstSPtr<Network::Session> Session) override
    {
        LOG_INFO("Someone connected...");
    }

    // -=(Undocumented)=-
    void OnDetach(ConstSPtr<Network::Session> Session) override
    {
        LOG_INFO("Someone disconnected...");
    }

    // -=(Undocumented)=-
    void OnError(ConstSPtr<Network::Session> Session, UInt Error, CStr Description) override
    {
        LOG_INFO("Error from the underlying network service '{}'", Description);
    }

    // -=(Undocumented)=-
    void OnRead(ConstSPtr<Network::Session> Session,  CPtr<UInt08> Bytes) override
    {
        LOG_INFO("Someone sent data...");
    }

    // -=(Undocumented)=-
    void OnWrite(ConstSPtr<Network::Session> Session, CPtr<UInt08> Bytes) override
    {
        LOG_INFO("We wrote some data...");
    }

private:

    SPtr<Network::Server> mEndpoint;
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