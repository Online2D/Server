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

#include <Network/Service.hpp>
#include "Foundation/Account/Account.hpp"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Foundation
{
    // -=(Undocumented)=-
    class Peer final
    {
    public:

        // -=(Undocumented)=-
        Peer(ConstSPtr<Network::Client> Connection);

        // -=(Undocumented)=-
        UInt GetID() const
        {
            return mConnection->GetID();
        }

        // -=(Undocumented)=-
        void SetAccount(ConstSPtr<Account> Account)
        {
            mAccount = Account;
        }

        // -=(Undocumented)=-
        ConstSPtr<Account> GetAccount() const
        {
            return mAccount;
        }

        // -=(Undocumented)=-
        template<typename Message>
        void Close(Message && Packet)
        {
            mConnection->Write(Packet, false);
            mConnection->Close(false);
        }

        // -=(Undocumented)=-
        void Close()
        {
            mConnection->Close(true);
        }

        // -=(Undocumented)=-
        template<typename Message>
        void Write(Message && Packet, Bool Unreliable = false)
        {
            if constexpr (std::is_base_of_v<Writer, Message>)
            {
                mConnection->Write(Packet.GetData(), Unreliable);
            }
            else
            {
                mConnection->Write(Packet, Unreliable);
            }
        }

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        SPtr<Network::Client> mConnection;
        SPtr<Account>         mAccount;
    };
}