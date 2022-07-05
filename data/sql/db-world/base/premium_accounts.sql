DELETE FROM `command` WHERE `name` IN ('premium account add', 'premium account remove', 'premium account addbit', 'premium account removebit');
INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('premium account add', 2, 'Syntax: .premium account add [$account]\r\n\r\nEnable premium features for the account'),
('premium account remove', 2, 'Syntax: .premium account remove [$account]\r\n\r\nDisable premium features for the account'),
('premium account addbit', 2, 'Syntax: .premium account addbit [$account] [$bit]\r\n\r\nAdd a bit to the account. To see a list of all available bits, use .premium listitems'),
('premium account removebit', 2, 'Syntax: .premium account removebit [$account] [$bit]\r\n\r\nRemove a bit from the account. To see a list of all available bits, use .premium listitems');
