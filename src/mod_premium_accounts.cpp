#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

int premiumRealmId;

class PremiumCommand : public CommandScript
{
public:
    PremiumCommand() : CommandScript("PremiumCommand") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable premiumAccountTable =
        {
            { "add", HandleAddPremiumAccountCommand, SEC_GAMEMASTER, Console::Yes },
            { "remove", HandleRemovePremiumAccountCommand, SEC_GAMEMASTER, Console::Yes },
        };

        static ChatCommandTable premiumCommandTable =
        {
            { "account", premiumAccountTable },
        };

        static ChatCommandTable commandTable =
        {
            { "premium", premiumCommandTable }
        };

        return commandTable;
    }

    static bool HandleAddPremiumAccountCommand(ChatHandler* handler, std::string const& name)
    {
        uint32 accountId = GetAccountId(name);

        if (accountId == 0)
        {
            SendMessage(handler, Acore::StringFormat("Couldn't find an account with the name %s.", ToUpper(name)));
            return true;
        }

        if (IsPremium(accountId))
        {
            SendMessage(handler, Acore::StringFormat("The account %s is already a premium account.", ToUpper(name)));
            return true;
        }

        SetPremiumAccount(accountId, false);

        SendMessage(handler, Acore::StringFormat("The account %s has been set to premium.", ToUpper(name)));
        return true;
    }

    static bool HandleRemovePremiumAccountCommand(ChatHandler* handler, std::string const& name)
    {
        uint32 accountId = GetAccountId(name);

        if (accountId == 0)
        {
            SendMessage(handler, Acore::StringFormat("Couldn't find an account with the name %s.", ToUpper(name)));
            return true;
        }

        if (!IsPremium(accountId))
        {
            SendMessage(handler, Acore::StringFormat("The account %s is not a premium account.", ToUpper(name)));
            return true;
        }

        SetPremiumAccount(accountId, true);

        SendMessage(handler, Acore::StringFormat("The account %s has been removed from the premium accounts.", ToUpper(name)));
        return true;
    }

private:
    static bool IsPremium(uint32 accountId)
    {
        QueryResult result = LoginDatabase.Query("SELECT * FROM `premium_accounts` WHERE `account_id` = {}", accountId);

        if (!result)
            return false;

        return true;
    }

    static uint32 GetAccountId(std::string name)
    {
        QueryResult result = LoginDatabase.Query("SELECT `id` FROM `account` WHERE `username` = '{}'", name);
        if (!result)
            return 0;

        Field* fields = result->Fetch();
        uint32 accountId = fields[0].Get<uint32>();

        return accountId;
    }

    static void SetPremiumAccount(uint32 accountId, bool remove)
    {
        std::string query = "";
        if (!remove)
        {
            query = Acore::StringFormat("REPLACE INTO `premium_accounts` (`account_id`) VALUES (%i)", accountId);
        }
        else
        {
            query = Acore::StringFormat("DELETE FROM `premium_accounts` WHERE `account_id` = %i", accountId);
        }

        QueryResult result = LoginDatabase.Query(query);
    }

    static void SendMessage(ChatHandler* handler, std::string text)
    {
        if (handler->IsConsole())
        {
            handler->SendSysMessage(text);
        }
        else
        {
            ChatHandler(handler->GetSession()).SendSysMessage(text);
        }
    }

    static std::string ToUpper(std::string text)
    {
        std::for_each(text.begin(), text.end(), [](char& c)
        {
            c = ::toupper(c);
        });

        return text;
    }

    static std::string ToLower(std::string text)
    {
        std::for_each(text.begin(), text.end(), [](char& c)
        {
            c = ::tolower(c);
        });

        return text;
    }
};

class PremiumCharacter : public PlayerScript
{
public:
    PremiumCharacter() : PlayerScript("PremiumCharacter") {}

    void OnLogin(Player* player) override
    {
    }

    void OnDelete(ObjectGuid guid, uint32 /*accountId*/) override
    {
        //QueryResult result = LoginDatabase.Query("DELETE FROM `premium_accounts_rewarded` WHERE `realm_id` = {} AND `character_guid` = {}", premiumRealmId, guid.GetCounter());
    }
};

class PremiumData : public WorldScript
{
public:
    PremiumData() : WorldScript("PremiumData") {}

    void OnAfterConfigLoad(bool reload) override
    {
        premiumRealmId = sConfigMgr->GetOption<uint32>("RealmID", 0);
    }
};

void AddPremiumAccountsScripts()
{
    new PremiumCharacter();
    new PremiumCommand();
    new PremiumData();
}
