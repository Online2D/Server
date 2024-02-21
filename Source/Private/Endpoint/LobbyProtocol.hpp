// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2024 by Online-MMO-Engine Team. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <Network/Client.hpp>
#include <Network/Protocol.hpp>
#include "LobbyPackets.hpp"
#include "Game/AccountServiceLocal.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Endpoint
{
    // -=(Undocumented)=-
    class LobbyProtocol final : public Network::Protocol
    {
    public:

        // -=(Undocumented)=-
        LobbyProtocol(ConstSPtr<Game::AccountServiceLocal> AccountService);

        // -=(Undocumented)=-
        void OnAttach(ConstSPtr<Network::Client> Client) override;

        // -=(Undocumented)=-
        void OnDetach(ConstSPtr<Network::Client> Client) override;

        // -=(Undocumented)=-
        void OnError(ConstSPtr<Network::Client> Client, UInt Error, CStr Description) override;

        // -=(Undocumented)=-
        void OnRead(ConstSPtr<Network::Client> Client,  CPtr<UInt08> Bytes) override;

        // -=(Undocumented)=-
        void OnWrite(ConstSPtr<Network::Client> Client, CPtr<UInt08> Bytes) override;

    private:

        // -=(Undocumented)=-
        void Handle_LobbyAccountLogin(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountLogin> Message);

        // -=(Undocumented)=-
        void Handle_LobbyAccountRegister(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountRegister> Message);

        // -=(Undocumented)=-
        void Handle_LobbyAccountDelete(ConstSPtr<Network::Client> Client, Ref<const LobbyAccountDelete> Message);

    private:

        // -=(Undocumented)=-
        Bool Validate_Username(CStr Username);

        // -=(Undocumented)=-
        Bool Validate_Password(CStr Password);

        // -=(Undocumented)=-
        Bool Validate_Email(CStr Email);

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        SPtr<Game::AccountServiceLocal> mAccountService;
    };
}