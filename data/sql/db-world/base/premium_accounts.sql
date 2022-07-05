DELETE FROM `command` WHERE `name` IN ('premium account add', 'premium account remove', 'premium account additem', 'premium account removeitem');
INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('premium account add', 2, 'Syntax: .premium account add [$account]\r\n\r\nEnable premium features for the account'),
('premium account remove', 2, 'Syntax: .premium account remove [$account]\r\n\r\nDisable premium features for the account'),
('premium account additem', 2, 'Syntax: .premium account additem [$account] [$bitmask]\r\n\r\nAdd an item to the account. To see a list of all available items, use .premium listitems'),
('premium account removeitem', 2, 'Syntax: .premium account removeitem [$account] [$bitmask]\r\n\r\nRemove an item from the account. To see a list of all available items, use .premium listitems');
