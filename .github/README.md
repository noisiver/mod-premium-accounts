# Premium Accounts
This will give players the ability earn special rewards. It could be used to reward players for donating or helping out in some way, but the use for it is up to you.

The rewards will be sent to every character on every realm on an account.

The rewarded items are stored as a bitmask and the items sent to the character are also stored as a bitmask to make sure the same reward isn't sent more than once to the same character.

# First use
When first installing the module, feel free to remove all the items from the config and add only those that you want. A few items were added as an example.

You can change the text sent to the player when items are mailed to them from the config.

# Warning
If you've used the module and want to add more items, **do not** add new item ids anywhere but at the end of the list. The way the module works is it loads the list of items and assigns them a bitmask which is used to determine what rewards a player has access to. Adding item ids anywhere but the end of the config option will mess with those bitmasks and change the items the player has earned and received.

**It's not recommended to use any item that is linked to a specific race, class or team.**

The module is still in development and shouldn't be used for production yet.
