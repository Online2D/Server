// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2024 by Agustin L. Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Endpoint.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Foundation
{
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Endpoint::Endpoint(Ref<Context> Context)
        : Subsystem(Context),
          mAccounts(Context)
    {
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnTick(Real64 Time)
    {
        // TODO: Detect idle peer(s) and kick them
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    Bool Endpoint::Listen(UInt16 Port, UInt Capacity)
    {
        mServer = GetSubsystem<Network::Service>()->Listen(* this, Port, Capacity);
        return (mServer != nullptr);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::Close()
    {
        mServer = nullptr;
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnConnect(ConstSPtr<Network::Client> Connection)
    {
        const SPtr<Peer> Session = NewPtr<Peer>(Connection);
        mRegistry.try_emplace(Connection->GetID(), Session);

        // TODO: OnConnect(Peer)
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnDisconnect(ConstSPtr<Network::Client> Connection)
    {
        const SPtr<Peer> Session = mRegistry.at(Connection->GetID());
        mRegistry.erase(Connection->GetID());

        // TODO Release the player attached to the peer

        // Release the account attached to the peer
        ConstSPtr<Account> Account = Session->GetAccount();
        if (Account)
        {
            mAccounts.Logout(Account->GetID());
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnRead(ConstSPtr<Network::Client> Connection, CPtr<UInt08> Bytes)
    {
        ConstSPtr<Peer> Session = mRegistry.at(Connection->GetID());

        Reader Archive(Bytes);
        do
        {
            switch (Archive.ReadInt<UInt>())
            {
            case GatewayAccountLogin::kID:
                OnHandle(Session, GatewayAccountLogin(Archive));
                break;
            case GatewayAccountRegister::kID:
                OnHandle(Session, GatewayAccountRegister(Archive));
                break;
            case GatewayAccountDelete::kID:
                OnHandle(Session, GatewayAccountDelete(Archive));
                break;
            case LobbyCharacterEnter::kID:
                OnHandle(Session, LobbyCharacterEnter(Archive));
                break;
            case LobbyCharacterDelete::kID:
                OnHandle(Session, LobbyCharacterDelete(Archive));
                break;
            case LobbyCharacterCreate::kID:
                OnHandle(Session, LobbyCharacterCreate(Archive));
                break;
            }
        }
        while (Archive.GetAvailable() > 0);
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnHandle(ConstSPtr<Peer> Session, Ref<const GatewayAccountLogin> Message)
    {
        const AccountService::Error Result = mAccounts.Login(Message.Username, Message.Password);
        if (Result == AccountService::Error::None)
        {
            Session->SetAccount(mAccounts.GetByUsername(Message.Username));
            Session->Write(GatewayAccountData()); // TODO: Send character
        }
        else
        {
            NotifyError(Session, Result);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnHandle(ConstSPtr<Peer> Session, Ref<const GatewayAccountRegister> Message)
    {
        const AccountService::Error Result = mAccounts.Create(Message.Username, Message.Password, Message.Email);
        if (Result == AccountService::Error::None)
        {
            Session->SetAccount(mAccounts.GetByUsername(Message.Username));
            Session->Write(GatewayAccountData()); // TODO: Send character
        }
        else
        {
            NotifyError(Session, Result);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnHandle(ConstSPtr<Peer> Session, Ref<const GatewayAccountDelete> Message)
    {
        const AccountService::Error Result = mAccounts.Delete(Message.Username, Message.Password);
        if (Result == AccountService::Error::None)
        {
            // TODO ....
        }
        else
        {
            NotifyError(Session, Result);
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::NotifyError(ConstSPtr<Peer> Session, AccountService::Error Error)
    {
        switch (Error)
        {
        case AccountService::Error::None:
            break;
        case AccountService::Error::Invalid:
            Session->Close(GatewayAccountError(GatewayAccountError::ID::Invalid));
            break;
        case AccountService::Error::Mismatch:
            Session->Close(GatewayAccountError(GatewayAccountError::ID::Mismatch));
            break;
        case AccountService::Error::Exist:
            Session->Close(GatewayAccountError(GatewayAccountError::ID::Exist));
            break;
        case AccountService::Error::Connected:
            Session->Close(GatewayAccountError(GatewayAccountError::ID::Online));
            break;
        case AccountService::Error::Disconnected:
            break;
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnHandle(ConstSPtr<Peer> Session, Ref<const LobbyCharacterEnter> Message)
    {
        if (Session->GetAccount())
        {
            // TODO Handle Message
        }
        else
        {
            Session->Close();
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnHandle(ConstSPtr<Peer> Session, Ref<const LobbyCharacterDelete> Message)
    {
        if (Session->GetAccount())
        {
            // TODO Handle Message
        }
        else
        {
            Session->Close();
        }
    }

    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    void Endpoint::OnHandle(ConstSPtr<Peer> Session, Ref<const LobbyCharacterCreate> Message)
    {
        if (Session->GetAccount())
        {
            // TODO Handle Message
        }
        else
        {
            Session->Close();
        }
    }

}