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
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    LobbyProtocol::LobbyProtocol(ConstSPtr<Game::AccountServiceLocal> AccountService)
        : mAccountService { AccountService }
    {

    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::OnAttach(ConstSPtr<Network::Client> Client)
    {
        LOG_INFO("LobbyProtocol::OnAttach");
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

        //TODO: Testing purpose
        SPtr<Game::Account> Account = mAccountService->GetByUsername(Message.Username);

        if (Account)
        {
            if (Account->GetPassword() == Message.Password)
            {
                LOG_INFO("{} logged!", Message.Username);
            }
            else
            {
                LOG_INFO("{} mismatch password", Message.Username);
            }
        }
        else
        {
            LOG_INFO("{} can't be found in the database", Message.Username);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::Handle_LobbyAccountRegister(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountRegister> Message)
    {
        LOG_INFO("LobbyProtocol::Handle_LobbyAccountRegister {}:{}:{}", Message.Username, Message.Password, Message.Email);

        //TODO: Testing purpose
        SPtr<Game::Account> Account = mAccountService->GetByUsername(Message.Username);
        if (Account)
        {
            LOG_INFO("{} failed to create: already exist", Message.Username);
        }
        else
        {
            Account = NewPtr<Game::Account>(1, Message.Username, Message.Password, Message.Email);
            if (mAccountService->Create(Account))
            {
                LOG_INFO("{} account created!", Message.Username);
            }
            else
            {
                LOG_INFO("{} failed to create", Message.Username);
            }
        }
    }

    void LobbyProtocol::Handle_LobbyAccountDelete(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountDelete> Message)
    {
        LOG_INFO("LobbyProtocol::Handle_LobbyAccountDelete {}", Message.Username);

        //TODO: Testing purpose
        SPtr<Game::Account> Account = mAccountService->GetByUsername(Message.Username);
        if (mAccountService->Delete(Account))
        {
            LOG_INFO("{} account delete!", Message.Username);
        }
        else
        {
            LOG_INFO("{} failed to delete", Message.Username);
        }
    }

}

