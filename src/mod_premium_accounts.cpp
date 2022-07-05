#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Tokenize.h"

using namespace Acore::ChatCommands;

uint32 allPremiumBitmask;
int premiumRealmId;
std::string premiumMailText;

struct PremiumItems
{
    std::string name;
    uint32 itemId;
    uint32 bitMask;
};

std::vector<PremiumItems> premiumItems;

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
            { "addbit", HandlePremiumAccountAddBitmaskCommand, SEC_GAMEMASTER, Console::Yes },
            { "removebit", HandlePremiumAccountRemoveBitmaskCommand, SEC_GAMEMASTER, Console::Yes },
        };

        static ChatCommandTable premiumCommandTable =
        {
            { "account", premiumAccountTable },
            { "listitems", HandleListPremiumItemsCommand, SEC_GAMEMASTER, Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "premium", premiumCommandTable }
        };

        return commandTable;
    }

    static bool HandleListPremiumItemsCommand(ChatHandler* handler)
    {
        SendMessage(handler, "Available items:");

        for (auto& item : premiumItems)
        {
            SendMessage(handler, Acore::StringFormat("Name: %s, Bitmask: %i.", item.name, item.bitMask));
        }

        SendMessage(handler, Acore::StringFormat("Use %i as bitmask if you want to add all items to an account.", allPremiumBitmask));
        return true;
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

    static bool HandlePremiumAccountAddBitmaskCommand(ChatHandler* handler, std::string const& accountName, uint32 bitmask)
    {
        uint32 accountId = GetAccountId(accountName);

        if (accountId == 0)
        {
            SendMessage(handler, Acore::StringFormat("Couldn't find an account with the name %s.", ToUpper(accountName)));
            return true;
        }

        if (!IsPremium(accountId))
        {
            SendMessage(handler, Acore::StringFormat("The account %s is not set as premium.", ToUpper(accountName)));
            return true;
        }

        uint32 enabledBitMask = GetAccountBitmask(accountId);

        if (bitmask == allPremiumBitmask)
        {
            if (enabledBitMask != allPremiumBitmask)
            {
                SendMessage(handler, Acore::StringFormat("Added all items to the account %s.", ToUpper(accountName)));
                SetAllAccountBitmask(accountId, false);
            }
            else
            {
                SendMessage(handler, Acore::StringFormat("The account %s already has all items enabled.", ToUpper(accountName)));
            }
        }
        else
        {
            for (auto& item : premiumItems)
            {
                if (bitmask == item.bitMask)
                {
                    if (enabledBitMask & item.bitMask)
                    {
                        SendMessage(handler, Acore::StringFormat("The item %s is already enabled for the account %s.", item.name, ToUpper(accountName)));
                    }
                    else
                    {
                        SendMessage(handler, Acore::StringFormat("Added %s to the account %s.", item.name, ToUpper(accountName)));
                        SetAccountBitmask(accountId, bitmask, false);
                    }
                    return true;
                }
            }

            SendMessage(handler, Acore::StringFormat("Couldn't find an item with a bitmask of %i.", bitmask));
        }
        return true;
    }

    static bool HandlePremiumAccountRemoveBitmaskCommand(ChatHandler* handler, std::string const& accountName, uint32 bitmask)
    {
        uint32 accountId = GetAccountId(accountName);

        if (accountId == 0)
        {
            SendMessage(handler, Acore::StringFormat("Couldn't find an account with the name %s.", ToUpper(accountName)));
            return true;
        }

        if (!IsPremium(accountId))
        {
            SendMessage(handler, Acore::StringFormat("The account %s is not set as premium.", ToUpper(accountName)));
            return true;
        }

        uint32 enabledBitMask = GetAccountBitmask(accountId);

        if (bitmask == allPremiumBitmask)
        {
            if (enabledBitMask != 0)
            {
                SendMessage(handler, Acore::StringFormat("Removed all items from the account %s.", ToUpper(accountName)));
                SetAllAccountBitmask(accountId, true);
            }
            else
            {
                SendMessage(handler, Acore::StringFormat("The account %s doesn't have any items enabled.", ToUpper(accountName)));
            }
        }
        else
        {
            for (auto& item : premiumItems)
            {
                if (bitmask == item.bitMask)
                {
                    if (enabledBitMask & item.bitMask)
                    {
                        SendMessage(handler, Acore::StringFormat("Removed %s from the account %s.", item.name, ToUpper(accountName)));
                        SetAccountBitmask(accountId, bitmask, true);
                    }
                    else
                    {
                        SendMessage(handler, Acore::StringFormat("The item %s is not enabled for the account %s.", item.name, ToUpper(accountName)));
                    }
                    return true;
                }
            }

            SendMessage(handler, Acore::StringFormat("Couldn't find an item with a bitmask of %i.", bitmask));
        }
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

    static void SetAccountBitmask(uint32 accountId, uint32 bitmask, bool remove)
    {
        std::string query = Acore::StringFormat("UPDATE `premium_accounts` SET `enabled_items` = `enabled_items` | %i WHERE `account_id` = %i", bitmask, accountId);

        if (remove)
            query = Acore::StringFormat("UPDATE `premium_accounts` SET `enabled_items` = `enabled_items` & ~%i WHERE `account_id` = %i", bitmask, accountId);

        QueryResult result = LoginDatabase.Query(query);
    }

    static void SetAllAccountBitmask(uint32 accountId, bool remove)
    {
        std::string query = Acore::StringFormat("UPDATE `premium_accounts` SET `enabled_items` = %i WHERE `account_id` = %i", allPremiumBitmask, accountId);

        if (remove)
            query = Acore::StringFormat("UPDATE `premium_accounts` SET `enabled_items` = 0 WHERE `account_id` = %i", accountId);

        QueryResult result = LoginDatabase.Query(query);
    }

    static uint32 GetAccountBitmask(uint32 accountId)
    {
        QueryResult result = LoginDatabase.Query("SELECT `enabled_items` FROM `premium_accounts` WHERE `account_id` = {}", accountId);

        if (!result)
            return 0;

        return result->Fetch()[0].Get<uint32>();
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
        if (IsPremium(player->GetSession()->GetAccountId()))
            SendAvailableItems(player);
    }

    void OnDelete(ObjectGuid guid, uint32 /*accountId*/) override
    {
        QueryResult result = LoginDatabase.Query("DELETE FROM `premium_accounts_rewarded` WHERE `realm_id` = {} AND `character_guid` = {}", premiumRealmId, guid.GetCounter());
    }

private:
    bool IsPremium(uint32 accountId)
    {
        QueryResult result = LoginDatabase.Query("SELECT * FROM `premium_accounts` WHERE `account_id` = {}", accountId);

        if (!result)
            return false;

        return true;
    }

    static uint32 GetAccountBitmask(uint32 accountId)
    {
        QueryResult result = LoginDatabase.Query("SELECT `enabled_items` FROM `premium_accounts` WHERE `account_id` = {}", accountId);

        if (!result)
            return 0;

        return result->Fetch()[0].Get<uint32>();
    }

    void SendAvailableItems(Player* player)
    {
        int enabledItems = GetAccountBitmask(player->GetSession()->GetAccountId());

        if (enabledItems > 0)
        {
            int requestedRewards = enabledItems;

            QueryResult result = LoginDatabase.Query("SELECT `items` FROM `premium_rewarded_characters` WHERE `realm_id` = {} AND `character_guid` = {}", premiumRealmId, player->GetGUID().GetCounter());

            if (result)
                requestedRewards = enabledItems & (~result->Fetch()[0].Get<uint32>());

            if (requestedRewards == 0)
                return;

            for (auto& item : premiumItems)
            {
                if (requestedRewards & item.bitMask)
                {
                    SendMailTo(player, item.name, premiumMailText, item.itemId, 1);
                }
            }

            result = LoginDatabase.Query("REPLACE INTO `premium_rewarded_characters` (`account_id`, `realm_id`, `character_guid`, `items`) VALUES ({}, {}, {}, {})", player->GetSession()->GetAccountId(), premiumRealmId, player->GetGUID().GetCounter(), enabledItems);
        }
    }

    void SendMailTo(Player* receiver, std::string subject, std::string body, uint32 itemId, uint32 itemCount)
    {
        uint32 guid = receiver->GetGUID().GetCounter();

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        MailDraft* mail = new MailDraft(subject, body);
        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemId);
        if (pProto)
        {
            Item* mailItem = Item::CreateItem(itemId, itemCount);
            if (mailItem)
            {
                mailItem->SaveToDB(trans);
                mail->AddItem(mailItem);
            }
        }

        mail->SendMailTo(trans, receiver ? receiver : MailReceiver(guid), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_RETURNED);
        delete mail;
        CharacterDatabase.CommitTransaction(trans);
    }
};

class PremiumWorld : public WorldScript
{
public:
    PremiumWorld() : WorldScript("PremiumServer") {}

    void OnAfterConfigLoad(bool reload) override
    {
        premiumMailText = sConfigMgr->GetOption<std::string>("PremiumAccounts.MailText", "Thank you for supporting our server!");
        premiumRealmId = sConfigMgr->GetOption<uint32>("RealmID", 0);

        if (reload)
            LoadPremiumItems();
    }

    void OnStartup() override
    {
        LoadPremiumItems();
    }

private:
    void LoadPremiumItems()
    {
        allPremiumBitmask = 0;
        premiumItems.clear();
        uint32 bitMask = 0;
        int i = 0;
        std::string stringItemIds(sConfigMgr->GetOption<std::string>("PremiumAccounts.Items", ""));
        for (std::string_view id : Acore::Tokenize(stringItemIds, ';', false))
        {
            uint32 entry = Acore::StringTo<uint32>(id).value_or(0);

            if (const ItemTemplate* item = sObjectMgr->GetItemTemplate(entry))
            {
                if (bitMask == 0)
                    bitMask = 1;
                else
                    bitMask = bitMask * 2;

                premiumItems.push_back(PremiumItems());
                premiumItems[i].name = item->Name1;
                premiumItems[i].itemId = entry;
                premiumItems[i].bitMask = bitMask;
                allPremiumBitmask |= premiumItems[i].bitMask;

                i++;
            }
        }
    }
};

void AddPremiumAccountsScripts()
{
    new PremiumCharacter();
    new PremiumCommand();
    new PremiumWorld();
}
