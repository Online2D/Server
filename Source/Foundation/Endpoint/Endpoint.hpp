// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2024 by Agustin L. Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Peer.hpp"
#include "Packet/GatewayPackets.hpp"
#include "Packet/LobbyPackets.hpp"

#include "Foundation/Account/AccountService.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Foundation
{
    // -=(Undocumented)=-
    class Endpoint final : public Subsystem, public Network::Protocol
    {
    public:

        // -=(Undocumented)=-
        Endpoint(Ref<Context> Context);

        // -=(Undocumented)=-
        void OnTick(Real64 Time) override;

        // -=(Undocumented)=-
        Bool Listen(UInt16 Port, UInt Capacity);

        // -=(Undocumented)=-
        void Close();

        // -=(Undocumented)=-
        ConstSPtr<Peer> GetPeer(UInt ID) const
        {
            return mRegistry.at(ID);
        }

        // -=(Undocumented)=-
        template<typename Message, typename Functor>
        void Send(Message && Packet, Functor OnFilter, Bool Unreliable)
        {
            // Dump message
            Writer Archive = Packet.Dump();

            // Write message to all session(s) filtered
            for (auto & [_, Peer] : mRegistry)
            {
                if (OnFilter(Peer))
                {
                    Peer->Write(Archive.GetData(), Unreliable);
                }
            }
        }

        // -=(Undocumented)=-
        template<typename Message>
        void SendAll(Message && Packet, Bool Unreliable)
        {
            constexpr auto OnFilter = [](ConstSPtr<Peer>)
            {
                return true;
            };
            Send(Packet, OnFilter, Unreliable);
        }

        // -=(Undocumented)=-
        template<typename Message>
        void SendExcept(Message && Packet, UInt Exception, Bool Unreliable)
        {
            constexpr auto OnFilter = [Exception](ConstSPtr<Peer> Peer)
            {
                return Peer->GetID() != Exception;
            };
            Send(Packet, OnFilter, Unreliable);
        }

    private:

        // \see Protocol::OnConnect
        void OnConnect(ConstSPtr<Network::Client> Connection) override;

        // \see Protocol::OnDisconnect
        void OnDisconnect(ConstSPtr<Network::Client> Connection) override;

        // \see Protocol::OnRead
        void OnRead(ConstSPtr<Network::Client> Connection, CPtr<UInt08> Bytes) override;

    private:

        // -=(Undocumented)=-
        void OnHandle(ConstSPtr<Peer> Session, Ref<const GatewayAccountLogin> Message);

        // -=(Undocumented)=-
        void OnHandle(ConstSPtr<Peer> Session, Ref<const GatewayAccountRegister> Message);

        // -=(Undocumented)=-
        void OnHandle(ConstSPtr<Peer> Session, Ref<const GatewayAccountDelete> Message);

        // -=(Undocumented)=-
        void NotifyError(ConstSPtr<Peer> Session, AccountService::Error Error);

        // -=(Undocumented)=-
        void OnHandle(ConstSPtr<Peer> Session, Ref<const LobbyCharacterEnter> Message);

        // -=(Undocumented)=-
        void OnHandle(ConstSPtr<Peer> Session, Ref<const LobbyCharacterDelete> Message);

        // -=(Undocumented)=-
        void OnHandle(ConstSPtr<Peer> Session, Ref<const LobbyCharacterCreate> Message);

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        SPtr<Network::Server>   mServer;
        Table<UInt, SPtr<Peer>> mRegistry;

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        AccountService          mAccounts;
    };
}