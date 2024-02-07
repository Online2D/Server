// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2024 by Online-MMO-Engine Team. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "LobbyProtocol.hpp"
#include <Core/Log/Service.hpp>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Endpoint
{

    LobbyProtocol::LobbyProtocol(ConstSPtr<Game::AccountServiceLocal> AccountService)
        : mAccountService{ AccountService }
    {

    }
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::OnAttach(ConstSPtr<Network::Client> Client)
    {
        LOG_INFO("LobbyProtocol::OnAttach");

        Client->Write(LobbyReady());
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::OnDetach(ConstSPtr<Network::Client> Client)
    {
        LOG_INFO("LobbyProtocol::OnDetach");
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::OnError(ConstSPtr<Network::Client> Client, UInt Error, CStr Description)
    {
        LOG_INFO("LobbyProtocol::OnError {}:{}", Error, Description);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::OnRead(ConstSPtr<Network::Client> Client, CPtr<UInt08> Bytes)
    {
#define HANDLE_PACKET(Name) \
        case Name::k_ID: { Name Packet; Packet.Decode(Serializer); Handle_ ## Name (Client, Packet); break; }

        Reader Serializer(Bytes);
        do
        {
            switch (Serializer.ReadInt<UInt>())
            {
                HANDLE_PACKET(LobbyAccountLogin)
                HANDLE_PACKET(LobbyAccountRegister)
                HANDLE_PACKET(LobbyAccountDelete)
            }
        }
        while (Serializer.GetAvailable() > 0);

    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::OnWrite(ConstSPtr<Network::Client> Client, CPtr<UInt08> Bytes)
    {
        LOG_INFO("LobbyProtocol::OnWrite");
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::Handle_LobbyAccountLogin(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountLogin> Message)
    {
        LOG_INFO("LobbyProtocol::Handle_LobbyAccountLogin {}:{}", Message.Username, Message.Password);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::Handle_LobbyAccountRegister(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountRegister> Message)
    {
        LOG_INFO("LobbyProtocol::Handle_LobbyAccountRegister {}:{}:{}", Message.Username, Message.Password, Message.Email);

        //TODO: Testing purpose
        SPtr<Game::Account> Account = NewPtr<Game::Account>(1, Message.Username, Message.Password);
        mAccountService->Create(Account);
    }

    void LobbyProtocol::Handle_LobbyAccountDelete(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountDelete> Message)
    {
        LOG_INFO("LobbyProtocol::Handle_LobbyAccountDelete {}", Message.Username);

        mAccountService->Delete(Message.Username);
    }

}

