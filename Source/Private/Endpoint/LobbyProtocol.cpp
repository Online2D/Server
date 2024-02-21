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
        LOG_DEBUG("LobbyProtocol::Handle_LobbyAccountLogin {}:{}", Message.Username, Message.Password);

        if (!Validate_Username(Message.Username) || !Validate_Password(Message.Password))
        {
            return;
        }

        const SPtr<Game::Account> Account = mAccountService->GetByUsername(Message.Username);

        if (Account && Account->GetPassword() == Message.Password)
        {
            Client->Write(LobbyAccountError(LobbyAccountError::ID::Invalid));
        }
        else
        {
            Client->Write(LobbyAccountError(LobbyAccountError::ID::InvalidCredentials));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::Handle_LobbyAccountRegister(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountRegister> Message)
    {
        LOG_DEBUG("LobbyProtocol::Handle_LobbyAccountRegister {}:{}:{}", Message.Username, Message.Password, Message.Email);

        if (!Validate_Username(Message.Username) || !Validate_Password(Message.Password) || !Validate_Email(Message.Email))
        {
            return;
        }

        SPtr<Game::Account> Account = mAccountService->GetByUsername(Message.Username);

        if (!Account)
        {
            Account = NewPtr<Game::Account>(1 /* TODO */, Message.Username, Message.Password, Message.Email);

            if (mAccountService->Create(Account))
            {
                // TODO: Created -> Login
            }
            else
            {
                Client->Write(LobbyAccountError(LobbyAccountError::ID::Invalid));
            }
        }
        else
        {
            Client->Write(LobbyAccountError(LobbyAccountError::ID::AlreadyExist));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void LobbyProtocol::Handle_LobbyAccountDelete(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountDelete> Message)
    {
        LOG_DEBUG("LobbyProtocol::Handle_LobbyAccountDelete {}:{}", Message.Username, Message.Password);

        if (!Validate_Username(Message.Username) || !Validate_Password(Message.Password))
        {
            return;
        }

        const SPtr<Game::Account> Account = mAccountService->GetByUsername(Message.Username);

        if (Account && Account->GetPassword() == Message.Password)
        {
            if (mAccountService->Delete(Account))
            {
                // TODO: Successful
            }
            else
            {
                Client->Write(LobbyAccountError(LobbyAccountError::ID::Invalid));
            }
        }
        else
        {
            Client->Write(LobbyAccountError(LobbyAccountError::ID::InvalidCredentials));
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool LobbyProtocol::Validate_Username(CStr Username)
    {
        return Username.size() >= 4 && Username.size() < 20;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool LobbyProtocol::Validate_Password(CStr Password)
    {
        return Password.size() == 32;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool LobbyProtocol::Validate_Email(CStr Email)
    {
        return Email.find_first_of('@') != CStr::npos;
    }
}

