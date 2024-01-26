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

#include <Network/Session.hpp>
#include <Network/Protocol.hpp>
#include "LobbyPackets.hpp"

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
        void OnAttach(ConstSPtr<Network::Session> Session) override;

        // -=(Undocumented)=-
        void OnDetach(ConstSPtr<Network::Session> Session) override;

        // -=(Undocumented)=-
        void OnError(ConstSPtr<Network::Session> Session, UInt Error, CStr Description) override;

        // -=(Undocumented)=-
        void OnRead(ConstSPtr<Network::Session> Session,  CPtr<UInt08> Bytes) override;

        // -=(Undocumented)=-
        void OnWrite(ConstSPtr<Network::Session> Session, CPtr<UInt08> Bytes) override;

    private:

        // -=(Undocumented)=-
        void Handle_LobbyAccountLogin(ConstSPtr<Network::Session> Session, Ref<const LobbyAccountLogin> Message);

        // -=(Undocumented)=-
        void Handle_LobbyAccountRegister(ConstSPtr<Network::Session> Session, Ref<const LobbyAccountRegister> Message);

    };
}